#include "IntersectionCalculator.hpp"
#include "MathematicalOperations.hpp"
#include "SceneTransformations.hpp"
#include "helper.hpp"
#include "parser.hpp"
#include <iostream>

IntersectionCalculator::IntersectionCalculator(const parser::Scene s) : scene(s){
    std::cout << "Creating IntersectionCalculator" << std::endl;
    precomputeAllVariables(scene);

}
IntersectionCalculator::~IntersectionCalculator(){
    std::cout << "Destroying IntersectionCalculator" << std::endl;
    freeNormalVectorMemory();
}

float IntersectionCalculator::intersectSphere(parser::Ray ray, parser::Sphere sphere) const{

    float A, B, C;  // ---> qudritic func constants

    float delta;

    parser::Vec3f scenter = scene.vertex_data[sphere.center_vertex_id -1];

    float sradius = sphere.radius;


    float t, t1, t2;

    C = (ray.a.x-scenter.x)*(ray.a.x-scenter.x) + (ray.a.y-scenter.y)*(ray.a.y-scenter.y) + (ray.a.z-scenter.z)*(ray.a.z-scenter.z) -sradius*sradius;

    B = 2*ray.b.x*(ray.a.x-scenter.x) + 2*ray.b.y*(ray.a.y-scenter.y) + 2*ray.b.z*(ray.a.z-scenter.z);

    A = ray.b.x*ray.b.x + ray.b.y*ray.b.y + ray.b.z*ray.b.z;

    delta = B*B-4*A*C;

    if(delta < 0 )
        return -1; // TODO - float comparison bazen yanlis sonuc verebilir sanki??
    else if(delta == 0 ){

        t = -B / (2*A);
    }else{

        float tmp;

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
            t = t1;
        }
    }

    return t;

}

std::pair<double, double> IntersectionCalculator::findSphereUandV(const parser::Sphere& sphere, parser::Vec3f point) const{
    const parser::Vec3f sphereCenter = scene.vertex_data[sphere.center_vertex_id-1];

    parser::Vec3f vectorToPoint = MatOp::vectorDifference(point, sphereCenter);
    vectorToPoint = MatOp::vectorNormalize(vectorToPoint);


    // Rotate et bakalim...
    vectorToPoint = SceneTransformations::rotateTextureVector(vectorToPoint, sphere.transformations, this->scene);


    double teta = acos(vectorToPoint.y); // teta is in radians.
    double fi   = atan2(vectorToPoint.z, vectorToPoint.x);


    double u = (-fi + M_PI)/ (2*M_PI);
    double v = teta/M_PI;


    std::pair<double, double> UandV;
    UandV.first = u;
    UandV.second = v;


    return UandV;
}


IntersectionCalculator::IntersectionData IntersectionCalculator::intersectRay(parser::Ray ray, float treshold) const{


    float tmin = INFINITY;

    IntersectionCalculator::IntersectionData intersection; // The attributes are the same for all three
    intersection.texture_ID = -1;
    float t1; // To find minimum

    const std::vector<parser::Sphere>& spheres = scene.spheres;

    for(int k = 0; k < spheres.size(); k++){

        t1 = intersectSphere(ray, spheres[k]);


        if(t1 >= treshold){ // BUNU NEDEN 1 diyoruz ya tam anlamadim?? --> treshold intersection pointin nerde
            //olacagini belirlerken kullaniliyor. 1 den buyuk oldugu durumda image planin gorunmeyen kismiyla ilgilenmiyoruz demek


            if(t1 < tmin){
                tmin =t1;


                parser::Vec3f point = MatOp::vectorAddition(ray.a, MatOp::vectorMultiplication(ray.b, tmin)); // find point on the object.

                parser::Vec3f sphereCenter = scene.vertex_data[spheres[k].center_vertex_id-1];
                parser::Vec3f normal = MatOp::vectorNormalize(MatOp::vectorAddition(point, MatOp::vectorMultiplication(sphereCenter, -1)));
                intersection.materialId = scene.spheres[k].material_id;
                intersection.texture_ID = scene.spheres[k].texture_id;
                if(intersection.texture_ID != -1){
                    //////////////////////
                    std::pair<double, double> UandV = findSphereUandV(scene.spheres[k], point);
                    intersection.UandV = UandV;
                    //////////////////////
                }

                intersection.normal     = normal;
                intersection.t = t1;

            }
        }
    }


    // Intersect triangle
    for(int k = 0; k < scene.triangles.size(); k++){

        // first one is t second one is uandv. However we may not use uandv depending on textureobjects in colorfunction
        std::pair<float, std::pair<double, double>> t_UandV = intersectTriangle(ray, scene.triangles[k].indices);
        t1 = t_UandV.first;
        //////////////////////////////
        if(t1 >= treshold){

            if(t1 < tmin){
                intersection.t          = t1;
                intersection.normal     = (*precomputedVariables.simpleTriangleNormalVectors)[k];
                intersection.materialId = scene.triangles[k].material_id;
                //////////////////////
                intersection.texture_ID = scene.triangles[k].texture_id;
                intersection.UandV = t_UandV.second;
                //////////////////////
                tmin = t1;

            }
        }
    }
    // Intersect meshes
    for(int k = 0; k < scene.meshes.size(); k++){
        std::pair<int, float> indexAndValueT;


        // Return the index that is closest and t value of the closes face
        std::pair<std::pair<int, float>, std::pair<double, double>> minimumData_UandV
        =  intersectMesh(ray, scene.meshes[k].faces, scene.meshes[k].texture_id, scene.vertex_data, tmin, treshold, k);

        indexAndValueT = minimumData_UandV.first;

        if(-1 != indexAndValueT.first){
            intersection.t          = indexAndValueT.second;
            intersection.normal     = (*(*precomputedVariables.meshNormalVectors)[k])[indexAndValueT.first];
            intersection.materialId = scene.meshes[k].material_id;
            //////////////////////////////
            intersection.texture_ID = scene.meshes[k].texture_id;
            intersection.UandV = minimumData_UandV.second;
            //////////////////////////////
            tmin = indexAndValueT.second;
        }
    }

    intersection.t = tmin;

    return intersection;

}


parser::Vec3f IntersectionCalculator::computeNormalVector(const parser::Face &face, const std::vector<parser::Vec3f> &vertex_data) const{
    // Triangle constants...
    // Notice the indices start from 1, so decrement is needed
    float x_a = vertex_data[face.v0_id-1].x;
    float y_a = vertex_data[face.v0_id-1].y;
    float z_a = vertex_data[face.v0_id-1].z;

    float x_b = vertex_data[face.v1_id-1].x;
    float y_b = vertex_data[face.v1_id-1].y;
    float z_b = vertex_data[face.v1_id-1].z;

    float x_c = vertex_data[face.v2_id-1].x;
    float y_c = vertex_data[face.v2_id-1].y;
    float z_c = vertex_data[face.v2_id-1].z;
    // Calculate the edge vectors
    // Direction is from second vertex to first and third
    parser::Vec3f edge1, edge2, normal;

    edge1.x = x_a - x_b;
    edge1.y = y_a - y_b;
    edge1.z = z_a - z_b;

    edge2.x = x_c - x_b;
    edge2.y = y_c - y_b;
    edge2.z = z_c - z_b;


    normal = MatOp::vectorNormalize(MatOp::vectorCrossProduct(edge2, edge1));

    return normal;

}
bool IntersectionCalculator::intersectBox(const parser::Ray &ray, const MinimumMaximumPoints &minimumMaximumPoints) const{
    /*
     Computer whether or not a ray intersects a box.

     Inspired from : https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
     */

    parser::Vec3f minimum = minimumMaximumPoints.minimum;
    parser::Vec3f maximum = minimumMaximumPoints.maximum;

    float tmin = (minimum.x - ray.a.x) / ray.b.x;
    float tmax = (maximum.x - ray.a.x) / ray.b.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (minimum.y - ray.a.y) / ray.b.y;
    float tymax = (maximum.y - ray.a.y) / ray.b.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (minimum.z - ray.a.z) / ray.b.z;
    float tzmax = (maximum.z - ray.a.z) / ray.b.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}
std::pair<double, double> calculateUandVForTriangleFace(parser::Scene scene, parser::Face face, float beta, float gamma ){
    std::pair<double, double> UandV;
    parser::Vec2f ua = face.ua;
    parser::Vec2f ub = face.ub;
    parser::Vec2f uc = face.uc;


    UandV.first = ua.x + beta * (ub.x - ua.x) + gamma * (uc.x - ua.x);
    UandV.second = ua.y + beta *(ub.y - ua.y) + gamma * (uc.y - ua.y);




    return UandV;
}
std::pair<float, std::pair<double, double>> IntersectionCalculator::intersectTriangle(const parser::Ray &ray, const parser::Face &face) const{
    /*
     Return t as float and normal vector as Vec3f.
     t = -1 if no intersection exists.

     (Snyder & Barr, 1987) method
     Textbook Page 78 notation used
     */

    // Some constants...
    const std::vector<parser::Vec3f> &vertexData = this->scene.vertex_data;

    // Ray constants...
    float x_e = ray.a.x;
    float y_e = ray.a.y;
    float z_e = ray.a.z;

    float x_d = ray.b.x;
    float y_d = ray.b.y;
    float z_d = ray.b.z;

    // Triangle constants...
    // Notice the indices start from 1, so decrement is needed
    float x_a = vertexData[face.v0_id-1].x;
    float y_a = vertexData[face.v0_id-1].y;
    float z_a = vertexData[face.v0_id-1].z;

    float x_b = vertexData[face.v1_id-1].x;
    float y_b = vertexData[face.v1_id-1].y;
    float z_b = vertexData[face.v1_id-1].z;

    float x_c = vertexData[face.v2_id-1].x;
    float y_c = vertexData[face.v2_id-1].y;
    float z_c = vertexData[face.v2_id-1].z;


    // Matrix elements...
    float a = x_a - x_b;
    float b = y_a - y_b;
    float c = z_a - z_b;

    float d = x_a - x_c;
    float e = y_a - y_c;
    float f = z_a - z_c;

    float g = x_d;
    float h = y_d;
    float i = z_d;

    // Solution variables...
    float j = x_a - x_e;
    float k = y_a - y_e;
    float l = z_a - z_e;


    // Composite variables...
    float M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);


    // Main variables
    float beta  = j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g);
    float gamma = i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c);
    float t     = f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c);

    // Normalization...
    beta  = beta  / M;
    gamma = gamma / M;
    t     = t     / M * (-1);



    //calculateUandVForTriangle
    std::pair<float, std::pair<double, double>> result;
    if(face.texture_id != -1){ // if there is o texture then there is no need to calculate uandv
        std::pair<double, double> UandV = calculateUandVForTriangleFace( scene,  face,  beta,  gamma );
        result.second = UandV;
    }

    /////////////////////////

    // TODO -> t_0 icin e noktasini, yani camera position aliyorum, bu yanlis olabilir mi?? cunku canvas ile kamera arasindaysa gormemeli sanirim? ya da gormeli mi :)
    if (
        t < 0     ||
        gamma < 0 || gamma + beta > 1 ||
        beta < 0
        ){
        // Will not use it anymore, return a null normal vector
        result.first = -1;
        return result;
    }else{
        result.first = t;
        return result;
    }


}
std::pair<std::pair<int, float>, std::pair<double, double>> IntersectionCalculator::intersectMesh(const parser::Ray &ray, const std::vector<parser::Face> &faces, int texture_id, const std::vector<parser::Vec3f> &vertexData, const float tThreshold, const float naturalThreshold, const int meshIndex) const{
    /*
     This function is actually a set of combination for triangles.
     The structure implies there is a vector of faces that specify 3 vertex coordinates.

     Given implementation is straightforward, iterate 'intersectTriangle' and return a vector of pairs which include
     t value and surface normal.

     Return only the t value and index after intersection is done. Notice the values greater than tThreshold will not be taken!
     */
    std::pair<int, float> minimumData;
    minimumData.first  = -1;
    minimumData.second = tThreshold;
    ///////////////////
    std::pair<std::pair<int, float>, std::pair<double, double>> minimumData_UandV;
    /////////////////////
    if (!intersectBox(ray, (*precomputedVariables.meshMinMaxPoints)[meshIndex])){
        minimumData_UandV.first = minimumData;
        minimumData_UandV.second.first = -58;// debugging
        minimumData_UandV.second.second = 58;// debugging
        return minimumData_UandV;
    }

    // Do for each triangle...
    for(int counter = 0; counter < faces.size(); counter++){
        ////////////
        std::pair<float, std::pair<double, double>> t_UandV = intersectTriangle(ray, faces[counter]);
        minimumData_UandV.second = t_UandV.second;
        ///////////

        float t = t_UandV.first;

        if(t >= naturalThreshold){
            if(t < minimumData.second){
                minimumData.first  = counter;
                minimumData.second = t;
            }
        }
    }

    minimumData_UandV.first = minimumData;

    return minimumData_UandV;


}



/*
 Precompute functions...
 */
void IntersectionCalculator::precomputeNormalVectors(const parser::Scene &scene){
    // Allocate for simple triangles
    precomputedVariables.simpleTriangleNormalVectors = new std::vector<parser::Vec3f>(scene.triangles.size());

    // Allocate for triangles inside a mesh
    precomputedVariables.meshNormalVectors = new std::vector<std::vector<parser::Vec3f>*>(scene.meshes.size());

    for(int triangleCounter = 0; triangleCounter < scene.triangles.size(); triangleCounter++){
        auto &face        = scene.triangles[triangleCounter].indices;
        auto &vertex_data = scene.vertex_data;

        (*precomputedVariables.simpleTriangleNormalVectors)[triangleCounter] = computeNormalVector(face, vertex_data);
    }

    // Precompute for meshes
    for(int meshCounter = 0; meshCounter < scene.meshes.size(); meshCounter++){
        // Iterate over triangles in each mesh
        (*precomputedVariables.meshNormalVectors)[meshCounter] = new std::vector<parser::Vec3f>(scene.meshes[meshCounter].faces.size());

        for(int triangleCounter = 0; triangleCounter < scene.meshes[meshCounter].faces.size(); triangleCounter++){
            auto &face        = scene.meshes[meshCounter].faces[triangleCounter];
            auto &vertex_data = scene.vertex_data;

            (*(*precomputedVariables.meshNormalVectors)[meshCounter])[triangleCounter] = computeNormalVector(face, vertex_data);
        }
    }
}
void IntersectionCalculator::precomputeMinMaxCoordinates(const parser::Scene &scene){
    /*
     Compute minimum and maximum x,y,z coordinates for bounding box
     for only individual objects.
     */
    precomputedVariables.meshMinMaxPoints = new std::vector<MinimumMaximumPoints>(scene.meshes.size());

    for(int meshCounter = 0; meshCounter < scene.meshes.size(); meshCounter++){
        // Iterate over faces to find minimum coordinates for each of x, y and z
        parser::Vec3f minimum, maximum;
        minimum.x = INFINITY;
        minimum.y = INFINITY;
        minimum.z = INFINITY;

        maximum.x = -INFINITY;
        maximum.y = -INFINITY;
        maximum.z = -INFINITY;

        for(int faceCounter = 0; faceCounter < scene.meshes[meshCounter].faces.size(); faceCounter++){
            parser::Face faceVertices = scene.meshes[meshCounter].faces[faceCounter];
            parser::Vec3f vertices[3];

            vertices[0] = scene.vertex_data[faceVertices.v0_id-1];
            vertices[1] = scene.vertex_data[faceVertices.v1_id-1];
            vertices[2] = scene.vertex_data[faceVertices.v2_id-1];

            // Iterate over each vertices
            for(int vertexCounter = 0; vertexCounter < 3; vertexCounter++){
                if(minimum.x > vertices[vertexCounter].x) minimum.x = vertices[vertexCounter].x;
                if(minimum.y > vertices[vertexCounter].y) minimum.y = vertices[vertexCounter].y;
                if(minimum.z > vertices[vertexCounter].z) minimum.z = vertices[vertexCounter].z;

                if(maximum.x < vertices[vertexCounter].x) maximum.x = vertices[vertexCounter].x;
                if(maximum.y < vertices[vertexCounter].y) maximum.y = vertices[vertexCounter].y;
                if(maximum.z < vertices[vertexCounter].z) maximum.z = vertices[vertexCounter].z;
            }

        }

        // Set back
        (*precomputedVariables.meshMinMaxPoints)[meshCounter].maximum = maximum;
        (*precomputedVariables.meshMinMaxPoints)[meshCounter].minimum = minimum;
    }
}
void IntersectionCalculator::precomputeAllVariables(const parser::Scene &scene){
    precomputeNormalVectors(scene);
    precomputeMinMaxCoordinates(scene); // Bounding box
}
void IntersectionCalculator::freeNormalVectorMemory(){
    // Free triangle vector
    delete precomputedVariables.simpleTriangleNormalVectors;

    // Free mesh vectors
    for(int meshCounter = 0; meshCounter < (*precomputedVariables.meshNormalVectors).size(); meshCounter++){
        delete (*precomputedVariables.meshNormalVectors)[meshCounter];
    }
    // Free also container of vectors
    delete precomputedVariables.meshNormalVectors;

    // Do not forget the min/max coordinates
    delete precomputedVariables.meshMinMaxPoints;
}
