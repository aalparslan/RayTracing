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
    return a.x*b.x + a.y*b.y + a.z*b.z;
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
    

    
    
    int max = cam.image_width * cam.image_height * 3;
    immage =  new unsigned char [ max];
    
    
}


void loadScene(parser::Scene * scene){
    
    
    scenePTR = scene;
    vertexData_PTR = scene->vertex_data.data(); // Bu pointer vertex data vectorunun basina point ediyor.
    
    numberOfSpheres = scene->spheres.size();
    numberOfTriangles = scene->triangles.size();
    numberOfMeshes = scene->meshes.size();
    
    
    spheres = new parser::Sphere[numberOfSpheres];
    triangles = new parser::Triangle[numberOfTriangles];
    meshes = new parser::Mesh[numberOfMeshes];
    
    
    
    for(int i =0; i < numberOfSpheres; i++){
        spheres[i].material_id = scene->spheres[i].material_id;
        spheres[i].center_vertex_id = scene->spheres[i].center_vertex_id;
        spheres[i].radius = scene->spheres[i].radius;
    }
    
    for(int i= 0; i < numberOfTriangles; i++){
        triangles[i].material_id = scene->triangles[i].material_id;
        triangles[i].indices = scene->triangles[i].indices; //copy by reference
    }
    
    for(int i = 0; i <numberOfMeshes; i++){
        meshes[i].material_id = scene->meshes[i].material_id;
        meshes[i].faces = scene->meshes[i].faces; // change it si copying deeply
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


void initImage(parser::Scene *scene){
    
    // Below sets the background color
    int  k =0;
    for(int i =0 ; i < cam.image_width; i++){
        for(int j =0; j < cam.image_height; j++){
            
            
            immage[k++] = (unsigned char) scene->background_color.x;
            immage[k++] = (unsigned char) scene->background_color.y;
            immage[k++] = (unsigned char) scene->background_color.z;
            
            
        }
    }
    
}

parser::Ray generateRay(int i, int j){
    
    parser::Ray tmp;
    // parser::Vec3f su, sv, s;
    tmp.a = cam.position;
    
    parser::Vec3f m;
    parser::Vec3f q;
    parser::Vec3f result;
    double su, sv;
    
    m = add(cam.position, mult(mult(cam.gaze, -1) , -cam.near_distance)); //gaze negatif olabilir kontrol et.
    q = add(m, add(mult(camUVector,cam.near_plane.x ), mult(cam.up,cam.near_plane.w) ));
    
    su = 0.5* pixelW + j*pixelW;
    sv = 0.5* pixelH + i*pixelH;
    
    //
    //    if(i >= 512 ){
    //        tmp.b.x = 0;
    //        tmp.b.y = 0 ;
    //        tmp.b.z = -1;
    //        return tmp;
    //    }
    result = add(q, add(mult(camUVector, su), mult(cam.up, -sv)));
    
    
    
    tmp.b = result;
    return tmp;
    
    
    
    //    su = mult(camUVector, cam.near_plane.x + (i* pixelW) + halfPixelW);
    //    sv = mult(cam.up, cam.near_plane.z + (j * pixelH) + halfPixelH);
    ////
    //    s = add(su, sv);
    
    //   tmp.b = add(mult(cam.gaze, cam.near_distance),s);
    
    
    return  tmp;
}

double intersectSphere(parser::Ray ray, parser::Sphere sphere){
    
    double A, B, C;  // ---> qudritic func constants
    
    double delta;
    
    parser::Vec3f scenter = vertexData_PTR[sphere.center_vertex_id -1];
    
    double sradius = sphere.radius;
    
    
    
    //parser::Vec3f p;
    double t, t1, t2;
    //int i;
    
    
    C = (ray.a.x-scenter.x)*(ray.a.x-scenter.x) + (ray.a.y-scenter.y)*(ray.a.y-scenter.y) + (ray.a.z-scenter.z)*(ray.a.z-scenter.z) -sradius*sradius;
    
    B = 2*ray.b.x*(ray.a.x-scenter.x) + 2*ray.b.y*(ray.a.y-scenter.y) + 2*ray.b.z*(ray.a.z-scenter.z);
    
    A = ray.b.x*ray.b.x + ray.b.y*ray.b.y + ray.b.z*ray.b.z;
    
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
        t2 = (-B - delta) / A;
        
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
    
    
    
    return  scenePTR->materials[materialID-1].ambient;
    
}

parser::Vec3f getDiffuseReflectance(int materialID){
    
    return scenePTR->materials[materialID-1].diffuse;
}

parser::Vec3f getspecularReflectance(int materialID){
    
    return scenePTR->materials[materialID-1].specular;
}


float getPhongExponent(int materialID){
    
    return scenePTR->materials[materialID-1].phong_exponent;
}



void printRay(parser::Ray ray){
    
    cout<< ray.a.x <<"," <<ray.a.y << ","<<ray.a.z << " ---> "<< ray.b.x <<"," <<ray.b.y << ","<<ray.b.z <<endl;
    
    
    
}


int clamp(float maximum,float givenColor){
    
    int result = (int) ((givenColor * 256) / maximum);
    
    return result;
    
}

void pixelColorSetToZero(int wherePixelStarts){
    immage[wherePixelStarts] = (unsigned char) 0;
    immage[wherePixelStarts +1] = (unsigned char) 0;
    immage[wherePixelStarts +2] = (unsigned char) 0;
}





int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    
    scene.loadFromXml(argv[1]);
    
    
    for(auto x = scene.cameras.begin(); x < scene.cameras.end(); x++){
        
        loadCamera(*x);
        loadScene(&scene);
        initImage(&scene);
        
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
                    
                    immage[wherePixelStarts] = (unsigned char) (ambienLight.x * ambientReflectance.x);//
                    immage[wherePixelStarts + 1] = (unsigned char) (ambienLight.y * ambientReflectance.y);// Add ambient component
                    immage[wherePixelStarts + 2] = (unsigned char) (ambienLight.z * ambientReflectance.z);//
                    
                    
                    
                    for(auto y = scene.point_lights.begin(); y < scene.point_lights.end(); y++){ //Her bir light source icin D ve S hesaplar
                        
                        
                        parser::Vec3f lightPosition = (*y).position;
                        parser::Vec3f lightIntensity = (*y).intensity;
                        parser::Vec3f point = add(ray.a, mult(ray.b, tmin)); // find point on the object.
                        
                        //                        cout << "point -> "  <<point.x << "," <<point.y <<"," <<point.z <<endl;
                        
                        parser::Vec3f sphereCenter = vertexData_PTR[spheres[closestObj].center_vertex_id-1];
                        
                        parser::Vec3f normal = add(point, mult(sphereCenter, -1)); // P - Center = Nomal vector
                        
                        //       cout << "normal before-> "  <<normal.x << "," <<normal.y <<"," <<normal.z <<endl;
                        
                        normal = normalize(normal);
                        
                        parser::Vec3f toLight = add(lightPosition, mult(point, -1)); //  L - P = toLight
                        //     cout << "tolight before-> "  <<toLight.x << "," <<toLight.y <<"," <<toLight.z <<endl;
                        
                        toLight = normalize(toLight);
                        
                        double cosTeta = dot(normal, toLight);
                        parser::Vec3f diffuseReflectance = getDiffuseReflectance(spheres[closestObj].material_id);
                        double lengthToLight = lengTh(toLight);
                        

                        if(lengthToLight < 1) {
                            lengthToLight = 1; // r 1 birimden yakinsa formuldeki diffuse sonsuza yaklasacagindan 1 den kucuk oldugunda 1 e sabitlendim.
                        }
                        
                        
                        
                        parser::Vec3f AttenuatedLightIntensity;
                        AttenuatedLightIntensity.x = lightIntensity.x/lengthToLight;
                        AttenuatedLightIntensity.y = lightIntensity.y/lengthToLight;
                        AttenuatedLightIntensity.z = lightIntensity.z/lengthToLight;
                        
                        
                        parser::Vec3f maxColor;
                        maxColor.x = 2*lightIntensity.x + ambienLight.x; // 2I_{attenuated} + Ambient Light = D + A + S
                        maxColor.y = 2*lightIntensity.y + ambienLight.y;
                        maxColor.z = 2*lightIntensity.z + ambienLight.z;
                        
                        float diffuseR =0;
                        float diffuseG =0;
                        float diffuseB =0;
                        
                        float spcecularR = 0;
                        float spcecularG = 0;
                        float spcecularB = 0;
                        
                        if(cosTeta > 0) {
                            
                            
                            
                            diffuseR =  (diffuseReflectance.x * cosTeta * AttenuatedLightIntensity.x / pow(lengthToLight, 2));//
                            diffuseG =  (diffuseReflectance.y * cosTeta * AttenuatedLightIntensity.y / pow(lengthToLight, 2));//
                            diffuseB =  (diffuseReflectance.z * cosTeta * AttenuatedLightIntensity.z / pow(lengthToLight, 2));//
                            
                            
                        }
                        
                        
                        parser::Vec3f toEye = add(cam.position, mult(point, -1)); // Camera - P = toEye
                        toEye = normalize(toEye);
                        parser::Vec3f halfVector = add(toEye, toLight);
                        halfVector = normalize(halfVector);
                        double consBeta = dot(normal, halfVector);
                        parser::Vec3f specularReflectance = getspecularReflectance(spheres[closestObj].material_id);
                        float phongExponent = getPhongExponent(spheres[closestObj].material_id);
                        
                        if(consBeta > 0){
                            
                        spcecularR =  (AttenuatedLightIntensity.x * specularReflectance.x * pow(consBeta, phongExponent));//
                        spcecularG =  (AttenuatedLightIntensity.y * specularReflectance.y * pow(consBeta, phongExponent));//
                        spcecularB =  (AttenuatedLightIntensity.z * specularReflectance.z * pow(consBeta, phongExponent));//
                            
                        }
                        
                        // cout<< "teta vs beta --> " << cosTeta << " vs " <<consBeta <<endl;
                        pixelColorSetToZero( wherePixelStarts);
                        

                        immage[wherePixelStarts] += (unsigned char) clamp(maxColor.x, ambienLight.x + diffuseR + spcecularR);
                        immage[wherePixelStarts+1] += (unsigned char) clamp(maxColor.y, ambienLight.y + diffuseG + spcecularG);
                        immage[wherePixelStarts +2] += (unsigned char) clamp(maxColor.z, ambienLight.z + diffuseB + spcecularB);
                        
                        if(consBeta > 0.9999 && scenePTR->materials[spheres[closestObj].material_id -1].diffuse.x == 1 && scenePTR->materials[spheres[closestObj].material_id -1].diffuse.y == 0 && scenePTR->materials[spheres[closestObj].material_id -1].diffuse.z == 0){
                            cout <<endl;
                            cout<<"R -->" << (int) immage[wherePixelStarts-2] << endl;
                            cout<< "G -->"  <<  (int) immage[wherePixelStarts-1] << endl;
                            cout<<"B -->"  <<  (int) immage[wherePixelStarts] << endl;
                            cout <<endl;
                        }

    
                    }
                    
                    
                    
                }
                
                
            }
        }
        
        write_ppm("test.ppm", immage, cam.image_width, cam.image_height);

    }
    
}

