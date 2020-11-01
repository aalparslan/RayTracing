#include <iostream>
#include "parser.hpp"
#include "ppm.hpp"
#include <math.h>


using namespace std;
typedef unsigned char RGB[3];

parser::Vec3f * vertexData_PTR;
 parser::Scene *scenePTR;

int numberOfSpheres;
int numberOfTriangles;
int numberOfMeshes;


parser::Sphere *spheres;
parser::Triangle *triangles;
parser::Mesh  *meshes;


parser::Camera cam;
parser::Vec3f camUVector;
//int sizeX, sizeY;
double pixelW, pixelH;
double halfPixelW, halfPixelH;



char outFileName[80];

//RGB **image;
unsigned char* immage;

parser::Vec3f cross1(parser::Vec3f a, parser::Vec3f b){
    parser::Vec3f tmp;
    tmp.x =  a.y*b.z-b.y*a.z;
    tmp.y = b.x*a.z-a.x*b.z;
    tmp.z = a.x*b.y-b.x*a.y;
    
    return tmp;
    
}

double lengTh(parser::Vec3f v){
    
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double dot(parser::Vec3f a, parser::Vec3f b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

parser::Vec3f normalize(parser::Vec3f v){
    
    parser::Vec3f tmp;
    double d;
    
    d = lengTh(v);
    tmp.x = v.x/d;
    tmp.y = v.y/d;
    tmp.z = v.z/d;
    
    return tmp;
    
    
}

void loadCamera(parser::Camera x){
    cam.position = x.position;
    cam.gaze = x.gaze;
    cam.up = x.up;
    cam.near_plane = x.near_plane;
    cam.near_distance = x.near_distance;
    cam.image_width = x.image_width;
    cam.image_height = x.image_height;
    cam.image_name = x.image_name;
    
    pixelW = (cam.near_plane.y - cam.near_plane.x)/ (double) cam.image_width;
    halfPixelW = pixelW * 0.5;
    
    
    
    pixelH = (cam.near_plane.w - cam.near_plane.z) / (double) cam.image_height;
    halfPixelH = pixelH * 0.5;
    
    
    camUVector = cross1(cam.gaze, cam.up);
    camUVector = normalize(camUVector);
    
    cam.up = cross1(camUVector, cam.gaze);
    cam.up = normalize(cam.up);
    
    cam.gaze = normalize(cam.gaze);
    
    
    immage =  new unsigned char [cam.image_width * cam.image_height * 3];

    
//    image = (RGB **) malloc(sizeof(RGB *) * cam.image_width);
//
//    if(image == NULL){
//        printf("Cannot allocate memory for image");
//        exit(1);
//    }
//
//    for(int i = 0; i < cam.image_width ; i++){
//        image[i] = (RGB *) malloc(sizeof(RGB)*cam.image_height);
//        if(image[i] == NULL){
//            printf("Cannot allocate memory for image.");
//            exit(1);
//        }
//    }
    
}


void loadScene(parser::Scene scene){
    
    scenePTR = &scene;
    vertexData_PTR = scene.vertex_data.data(); // Bu pointer vertex data vectorunun basina point ediyor.
    
    
    numberOfSpheres = scene.spheres.size();
    numberOfTriangles = scene.triangles.size();
    numberOfMeshes = scene.meshes.size();
    
//    spheres = (parser::Sphere * )malloc(sizeof(parser::Sphere)* numberOfSpheres);
//    triangles = (parser::Triangle *)malloc(sizeof(parser::Triangle)* numberOfTriangles);
//    meshes = (parser::Mesh *)malloc(sizeof(parser::Mesh)* numberOfMeshes);
    
    spheres = new parser::Sphere[numberOfSpheres];
    triangles = new parser::Triangle[numberOfTriangles];
    meshes = new parser::Mesh[numberOfMeshes];
    

    
    for(int i =0; i < numberOfSpheres; i++){
        spheres[i].material_id = scene.spheres[i].material_id;
        spheres[i].center_vertex_id = scene.spheres[i].center_vertex_id;
        spheres[i].radius = scene.spheres[i].radius;
    }
    
    for(int i= 0; i < numberOfTriangles; i++){
        triangles[i].material_id = scene.triangles[i].material_id;
        triangles[i].indices = scene.triangles[i].indices; //copy by reference
    }
    
    for(int i = 0; i <numberOfMeshes; i++){
        meshes[i].material_id = scene.meshes[i].material_id;
//        int numberOfelements = sizeof(scene.meshes)/ sizeof(scene.meshes[0]);
//        copy(meshes[i].faces, meshes[i].faces + numberOfelements, scene.meshes)
     
        meshes[i].faces = scene.meshes[i].faces; // change it si copying deeply
    }
  
}

parser::Vec3f mult(parser::Vec3f a, double c){
    
    parser::Vec3f tmp;
    
    tmp.x = a.x * c;
    tmp.y = a.y * c;
    tmp.z = a.z * c;
    
    return tmp;
}

parser::Vec3f add(parser::Vec3f a, parser::Vec3f b){
    parser::Vec3f tmp;
    
    tmp.x = a.x + b.x;
    tmp.y = a.y + b.y;
    tmp.z = a.z + b.z;
    
    return tmp;
}


void initImage(parser::Scene scene){
    
    // Below sets the background color
   int  k =0;
    for(int i =0 ; i < cam.image_width; i++){
        for(int j =0; j < cam.image_height; j++){
            
            immage[k++] = scene.background_color.x;
            immage[k++] = scene.background_color.y;
            immage[k++] = scene.background_color.z;
            
            
          
            
//            image[i][j][0] = scene.background_color.x;
//            image[i][j][1] = scene.background_color.y;
//            image[i][j][2] = scene.background_color.z;
        }
    }
    
}

parser::Ray generateRay(int i, int j){
    
    parser::Ray tmp;
    parser::Vec3f su, sv, s;
    
    tmp.a = cam.position;
    
    su = mult(camUVector, cam.near_plane.x + (i* pixelW) + halfPixelW);
    sv = mult(cam.up, cam.near_plane.z + (j * pixelH) + halfPixelH);
    
    s = add(su, sv);
    
    tmp.b = add(mult(cam.gaze, cam.near_distance),s);
    
    
    return  tmp;
}

double intersectSphere(parser::Ray ray, parser::Sphere sphere){
    
    double A, B, C;  // ---> qudritic func constants
    
    double delta;
    
    parser::Vec3f scenter = vertexData_PTR[sphere.center_vertex_id];
    double sradius = sphere.radius;
    
    //parser::Vec3f p;
    double t, t1, t2;
    //int i;
    
    
    C = (ray.a.x-scenter.x)*(ray.a.x-scenter.x)+(ray.a.y-scenter.y)*(ray.a.y-scenter.y)+(ray.a.z-scenter.z)*(ray.a.z-scenter.z)-sradius*sradius;
    
    B = 2*ray.b.x*(ray.a.x-scenter.x)+2*ray.b.y*(ray.a.y-scenter.y)+2*ray.b.z*(ray.a.z-scenter.z);
    
    A = ray.b.x*ray.b.x+ray.b.y*ray.b.y+ray.b.z*ray.b.z;

    delta = B*B-4*A*C;

    if(delta < 0 )
        return -1;
    else if(delta == 0 ){
        
        t = -B / (2*A);
    }else{
        
        double tmp;
        delta = sqrt(delta);
        A = 2*A;
        t1 = (-B + delta) / A;
        t2 = (-B + delta) / A;
        
        if(t2 < t1){
            tmp = t2;
            t2 = t1;
            t1 = tmp;
        }
        
        if(t1 >= 1.0 )
            t = t1;
        else{
            t = -1;
        }
    }
    
    return t;
    
}

parser::Vec3f getAmbientReflectance(int materialID){
    
    return  scenePTR->materials[materialID].ambient;
    
}

parser::Vec3f getDiffuseReflectance(int materialID){
    
    return scenePTR->materials[materialID].diffuse;
}

parser::Vec3f getspecularReflectance(int materialID){
    
    return scenePTR->materials[materialID].specular;
}


float getPhongExponent(int materialID){
    
    return scenePTR->materials[materialID].phong_exponent;
}


int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    
    scene.loadFromXml(argv[1]);
    

    
    for(auto x = scene.cameras.begin(); x < scene.cameras.end(); x++){
    
        loadCamera(*x);
        loadScene(scene);
        initImage(scene);
        
        cout<< cam.image_height <<endl;
        
        for (int i = 0; i < cam.image_width; i++){
            for(int j = 0; j < cam.image_height; j++){
                
                
                parser::Ray ray;
                double tmin = 40000; // Burayi kontrol et gerek var mi yada yeterli mi ?
                int closestObj  = -1;
                
                ray = generateRay(i,j);
                
                for(int k = 0; k < numberOfSpheres; k++){
                    
                    double t;
                    t = intersectSphere(ray, spheres[k]);
                    
                    if(t >= 1){
                        
                        if(t < tmin){
                            
                            tmin =t;
                            closestObj = k;
                        }
                    }
                }
                
                if (closestObj != -1){
                    
                    int wherePixelStarts = i*(cam.image_width)*3 + j*3;
                    
                    
                    
                    
                    parser::Vec3f ambientReflectance = getAmbientReflectance(spheres[closestObj].material_id);
                    parser::Vec3f ambienLight = scenePTR->ambient_light;
                    
                    immage[wherePixelStarts] = ambienLight.x * ambientReflectance.x;//
                    immage[wherePixelStarts + 1] = ambienLight.y * ambientReflectance.y;// Add ambient component
                    immage[wherePixelStarts + 2] = ambienLight.z * ambientReflectance.z;//
                    
//                    image[i][j][0] += ambienLight.x * ambientReflectance.x;
//                    image[i][j][1] += ambienLight.y * ambientReflectance.y;
//                    image[i][j][2] += ambienLight.z * ambientReflectance.z;
                    
                    for(auto y = scene.point_lights.begin(); y < scene.point_lights.end(); y++){ //Her bir light source icin D ve S hesaplar
                        
                        
                        parser::Vec3f lightPosition = (*y).position;
                        parser::Vec3f lightIntensity = (*y).intensity;
                        parser::Vec3f point = add(ray.a, mult(ray.b, tmin)); // find point on the object.
                        parser::Vec3f sphereCenter = vertexData_PTR[spheres[closestObj].center_vertex_id];
                        parser::Vec3f normal = add(point, mult(sphereCenter, -1)); // P - Center = Nomal vector
                        normal = normalize(normal);
                        
                        parser::Vec3f toLight = add(lightPosition, mult(point, -1)); //  L - P = toLight
                        toLight = normalize(toLight);
                        
                        double cosTeta = dot(normal, toLight);
                        parser::Vec3f diffuseReflectance = getDiffuseReflectance(spheres[closestObj].material_id);
                        double lengthToLight = lengTh(toLight);
                        
                        
                        immage[wherePixelStarts] += diffuseReflectance.x * cosTeta * lightIntensity.x / pow(lengthToLight, 2);//
                        immage[wherePixelStarts + 1] += diffuseReflectance.x * cosTeta * lightIntensity.x / pow(lengthToLight, 2);//
                        immage[wherePixelStarts + 2] += diffuseReflectance.z * cosTeta * lightIntensity.z / pow(lengthToLight, 2);//

//
//                        image[i][j][0] += diffuseReflectance.x * cosTeta * lightIntensity.x / pow(lengthToLight, 2);//
//                        image[i][j][1] += diffuseReflectance.x * cosTeta * lightIntensity.x / pow(lengthToLight, 2);//
//                        image[i][j][2] += diffuseReflectance.z * cosTeta * lightIntensity.z / pow(lengthToLight, 2);//
//
                        
                        parser::Vec3f toEye = add(cam.position, mult(point, -1)); // Camera - P = toEye
                        toEye = normalize(toEye);
                        parser::Vec3f halfVector = add(toEye, toLight);
                        double consBeta = dot(normal, halfVector);
                        parser::Vec3f specularReflectance = getspecularReflectance(spheres[closestObj].material_id);
                        float phongExponent = getPhongExponent(spheres[closestObj].material_id);

                        immage[wherePixelStarts] += lightIntensity.x * specularReflectance.x * pow(consBeta, phongExponent);//
                        immage[wherePixelStarts + 1] += lightIntensity.y * specularReflectance.y * pow(consBeta, phongExponent);// Specular component is calculated
                        immage[wherePixelStarts + 2] += lightIntensity.z * specularReflectance.z * pow(consBeta, phongExponent);//
                        
//                        image[i][j][0] += lightIntensity.x * specularReflectance.x * pow(consBeta, phongExponent);//
//                        image[i][j][1] += lightIntensity.y * specularReflectance.y * pow(consBeta, phongExponent);// Specular component is calculated
//                        image[i][j][2] += lightIntensity.z * specularReflectance.z * pow(consBeta, phongExponent);//
//
                        
  
                        
                    }
                    
                    
                }
                
                
            }
        }
        
        write_ppm("test.ppm", immage, cam.image_width, cam.image_height);

        
        
        
    }
   

    

    
    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.
    
//
//
//    const RGB BAR_COLOR[8] =
//    {
//        { 255, 255, 255 },  // 100% White
//        { 255, 255,   0 },  // Yellow
//        {   0, 255, 255 },  // Cyan
//        {   0, 255,   0 },  // Green
//        { 255,   0, 255 },  // Magenta
//        { 255,   0,   0 },  // Red
//        {   0,   0, 255 },  // Blue
//        {   0,   0,   0 },  // Black
//    };
//
//    int width = 640, height = 480;
//    int columnWidth = width / 8;
//
//    unsigned char* image = new unsigned char [width * height * 3];
//
//    int i = 0;
//    for (int y = 0; y < height; ++y)
//    {
//        for (int x = 0; x < width; ++x)
//        {
//            int colIdx = x / columnWidth;
//            image[i++] = BAR_COLOR[colIdx][0];
//            image[i++] = BAR_COLOR[colIdx][1];
//            image[i++] = BAR_COLOR[colIdx][2];
//        }
//    }
//
//    write_ppm("test.ppm", immage, cam.image_width, cam.image_height);
    
}

