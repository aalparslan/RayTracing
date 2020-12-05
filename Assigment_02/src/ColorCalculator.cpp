#include "ColorCalculator.hpp"
#include "MathematicalOperations.hpp"

#include <iostream>

using namespace std;

#define INFINITE 40000


parser::Vec3f ColorCalculator::getAmbientReflectance(int materialID){
    
    return  scenePTR->materials[materialID-1].ambient;
    
}

parser::Vec3f ColorCalculator::getDiffuseReflectance(int materialID){
    
    return scenePTR->materials[materialID-1].diffuse;
}

parser::Vec3f ColorCalculator::getspecularReflectance(int materialID){
    
    return scenePTR->materials[materialID-1].specular;
}


float ColorCalculator::getPhongExponent(int materialID){
    
    return scenePTR->materials[materialID-1].phong_exponent;
}




parser::Vec3f ColorCalculator::computeLightContribution(parser::Vec3f point, parser::PointLight light){
    
    //E(d) == I / d^2
    
    parser::Vec3f lightDirection = MatOp::vectorSubtraction(light.position, point);
    float lightDistance = MatOp::vectorLength(lightDirection);
    parser::Vec3f irradianceContribution = MatOp::vectorDivision(light.intensity, lightDistance * lightDistance);
    return irradianceContribution;
    
}

parser::Vec3f ColorCalculator::computeDiffuse(int materialID, parser::Vec3f normal, parser::Vec3f normalizedLightDirection, parser::Vec3f irradiance ){
    
    normal = MatOp::vectorNormalize(normal);
    float cosTheta = fmax(0.0f,  MatOp::dot(normalizedLightDirection, normal));
    
    
    parser::Vec3f diffuse = MatOp::vectorMultiplication(irradiance, cosTheta);
    parser::Vec3f diffuseReflectance = getDiffuseReflectance(materialID);
    // Multiplying with kd
    diffuse.x *= diffuseReflectance.x;
    diffuse.y *= diffuseReflectance.y;
    diffuse.z *= diffuseReflectance.z;
    
    return diffuse;
}


parser::Vec3f ColorCalculator::computeSpecular(int materialID, parser::Vec3f normal, parser::Vec3f irradiance, parser::Vec3f normalizedHalfVector ){
    
    float phongEx = scenePTR->materials[materialID-1].phong_exponent;
    float phongExponentCosAlpha =  pow(max(0.0f,(float) MatOp::dot(MatOp::vectorNormalize(normal), normalizedHalfVector)), phongEx );
    // (cosAlpha)^ns * E(d)
    parser::Vec3f specular = MatOp::vectorMultiplication(irradiance, phongExponentCosAlpha);
    parser::Vec3f specularReflectance = getspecularReflectance(materialID);
    
    // Multiplying with specular coeff
    specular.x *= specularReflectance.x;
    specular.y *= specularReflectance.y;
    specular.z *= specularReflectance.z;
    
    return specular;
}



parser::Vec3f ColorCalculator::computeColor(parser::Ray ray, IntersectionCalculator::IntersectionData intersection, int recursionNumber,parser::Scene *scene, IntersectionCalculator &ic){
    
    parser::Vec3f pixelColor = {};
    int materialID = intersection.materialId;
    parser::Vec3f point = MatOp::vectorAddition(ray.a, MatOp::vectorMultiplication(ray.b, intersection.t)); // find point on the object.
    parser::Vec3f eyeVector = MatOp::vectorSubtraction(ray.a, point);
    parser::Vec3f normalizedEyeVector = MatOp::vectorNormalize(eyeVector);
    parser::Vec3f ambientReflectance = getAmbientReflectance(materialID);
    parser::Vec3f ambienLight = scenePTR->ambient_light;
    pixelColor.x = (ambienLight.x * ambientReflectance.x);
    pixelColor.y = (ambienLight.y * ambientReflectance.y);
    pixelColor.z = (ambienLight.z * ambientReflectance.z);
    
    
    for(auto y = scene->point_lights.begin(); y < scene->point_lights.end(); y++){ //Her bir light source icin D ve S hesaplar
        
        parser::Vec3f lightPosition = (*y).position;
        parser::Vec3f lightDirection = MatOp::vectorAddition(lightPosition, MatOp::vectorMultiplication(point, -1));
        parser::Vec3f normalizedLightDirection = MatOp::vectorNormalize(lightDirection); //  L - P = toLight
        parser::Ray shadowRay;
        parser::Vec3f intOffset = MatOp::vectorMultiplication(normalizedLightDirection, scenePTR->shadow_ray_epsilon);
        shadowRay.a = MatOp::vectorAddition(point, intOffset);
        shadowRay.b = normalizedLightDirection;
        float shadowIntersection = ic.intersectRay(shadowRay, 0).t; //treshold 0 cunku belli bir noktanin otesindeki kesisimleri isteme gibi bir sartimiz yok
        
        if(shadowIntersection >= MatOp::vectorLength(lightDirection)){
            
            
            parser::Vec3f irradiance = computeLightContribution(point, (*y));
            parser::Vec3f diffuseContribution = computeDiffuse(materialID,  intersection.normal, normalizedLightDirection, irradiance );
            
            pixelColor = MatOp::vectorAddition(diffuseContribution, pixelColor); // add diffuse contribution to the pixel color
            
            // compute specular contribution
            parser::Vec3f normalizedHalfVector = MatOp::vectorNormalize(MatOp::vectorAddition(normalizedLightDirection, normalizedEyeVector));
            parser::Vec3f specularContribution = computeSpecular(materialID, intersection.normal, irradiance, normalizedHalfVector );
            pixelColor = MatOp::vectorAddition(specularContribution, pixelColor); // add specular contribution to the pixel color
        }
        else{
            
            continue;
        }
        
    }
    
    parser::Vec3f mirrorComponenet = scene->materials[materialID-1].mirror;
    
    if(recursionNumber > 0 && (mirrorComponenet.x > 0 || mirrorComponenet.y > 0 || mirrorComponenet.z > 0)){
        
        parser::Ray reflectedRay;
        float cosTheta = MatOp::dot(intersection.normal, normalizedEyeVector);
        reflectedRay.b = MatOp::vectorAddition(MatOp::vectorMultiplication(normalizedEyeVector, -1), MatOp::vectorMultiplication(intersection.normal, 2*cosTheta));
        reflectedRay.a = MatOp::vectorAddition(point ,MatOp::vectorMultiplication(reflectedRay.b, scenePTR->shadow_ray_epsilon) );
        IntersectionCalculator::IntersectionData reflectedIntersection = ic.intersectRay(reflectedRay,0); //treshold 0 cunku belli bir noktanin otesindeki kesisimleri isteme gibi bir sartimiz yok
        
        if(reflectedIntersection.t < INFINITE && reflectedIntersection.t > 0){ //  ray hits an object
            
            //parser::Ray ray, IntersectionCalculator::IntersectionData intersection, int recursionNumber,parser::Scene *scene
            parser::Vec3f reflectedRadiance = computeColor(reflectedRay, reflectedIntersection, recursionNumber-1, scene, ic);
            reflectedRadiance.x *= mirrorComponenet.x;
            reflectedRadiance.y *= mirrorComponenet.y;
            reflectedRadiance.z *= mirrorComponenet.z;
            pixelColor = MatOp::vectorAddition(reflectedRadiance, pixelColor);
        }
    }
    
    pixelColor.x = min(max(0.0f, pixelColor.x), 255.0f);
    pixelColor.y = min(max(0.0f, pixelColor.y), 255.0f);
    pixelColor.z = min(max(0.0f, pixelColor.z), 255.0f);
    return pixelColor;
}



void ColorCalculator::loadScene(parser::Scene * scene){
    
    
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
        triangles[i].indices = scene->triangles[i].indices;
    }
    
    for(int i = 0; i <numberOfMeshes; i++){
        meshes[i].material_id = scene->meshes[i].material_id;
        meshes[i].faces = scene->meshes[i].faces;
    }
    
}

ColorCalculator::ColorCalculator(parser::Scene *scene){
    std::cout << "Creating ColorCalculator" << std::endl;
    loadScene(scene);
}
ColorCalculator::~ColorCalculator(){
    std::cout << "Destroying ColorCalculator" << std::endl;
    delete[]  spheres;
    delete[]  triangles;
    delete[]  meshes;
}
