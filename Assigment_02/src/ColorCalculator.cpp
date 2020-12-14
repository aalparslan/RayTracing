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

//parser::Vec3f ColorCalculator::computeDiffuse(int materialID, parser::Vec3f normal, parser::Vec3f normalizedLightDirection, parser::Vec3f irradiance ){
//
//    normal = MatOp::vectorNormalize(normal);
//    float cosTheta = fmax(0.0f,  MatOp::dot(normalizedLightDirection, normal));
//
//
//    parser::Vec3f diffuse = MatOp::vectorMultiplication(irradiance, cosTheta);
//    parser::Vec3f diffuseReflectance = getDiffuseReflectance(materialID);
//    // Multiplying with kd
//    diffuse.x *= diffuseReflectance.x;
//    diffuse.y *= diffuseReflectance.y;
//    diffuse.z *= diffuseReflectance.z;
//
//    return diffuse;
//}


parser::Vec3f ColorCalculator::computeDiffuse( parser::Vec3f texelConstant, parser::Vec3f normal,
                                              parser::Vec3f normalizedLightDirection, parser::Vec3f irradiance ){
    
    normal = MatOp::vectorNormalize(normal);
    float cosTheta = fmax(0.0f,  MatOp::dot(normalizedLightDirection, normal));
    parser::Vec3f diffuse = MatOp::vectorMultiplication(irradiance, cosTheta);
    parser::Vec3f diffuseReflectance = texelConstant; // kd yerine bu kullanildi bu hem texelin colini hemde kd yi iceriyor.
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


parser::Vec3f ColorCalculator::CalculateTCC(int textureid,IntersectionCalculator::IntersectionData intersection, parser::Vec3f kd,
                                            vector<parser::TextureObject> textureObjects){
    
    if(textureid == -1 || textureObjects.size() < 1){ // no texture
        return kd;
    }
    
    
    
    parser::Texture texture = scenePTR->textures[textureid-1];
    float u = intersection.UandV.first;
    float v = intersection.UandV.second;
    
    int width = textureObjects[textureid-1].getWidth();
    int height = textureObjects[textureid-1].getHeight();
    unsigned char** image;
    image = (textureObjects[textureid-1].getImage());
    
    
    if(texture.appearance == "clamp"){
        u = fmax(0., fmin(1., u));
        v = fmax(0., fmin(1., v));
    }else{
        u -= floor(u);
        v -= floor(v);
    }
    
    if( u > 1 || u < 0){
        std::cout << "There is a mistake!"<<std::endl;
    }
    if( v > 1 || v < 0){
        std::cout << "There is a mistake!"<<std::endl;
    }
    
    
    
    u *= width;
    if (u >= width) u--;
    v *= height;
    if (v >= height) v--;
    parser::Vec3f TColor;
    
    if (texture.interpolation == "nearest" || ((v * width * 3 + u * 3) + 3 + width * 3 > width*height*3)) {
        
        const unsigned int position = ((int)v) * width * 3 + ((int)u) * 3;
        TColor.x = (*image)[position];
        TColor.y = (*image)[position +1];
        TColor.z = (*image)[position +2];
    } else { //bilinear
        const unsigned int p = u;
        const unsigned int q = v;
        const float dx = u - p;
        const float dy = v - q;
        const unsigned int position = q * width * 3 + p * 3;
        
        TColor.x = (*image)[position] * (1 - dx) * (1 - dy);
        TColor.x += (*image)[position + 3] * (dx) * (1 - dy);
        TColor.x += (*image)[position + 3 + width * 3] * (dx) * (dy);
        TColor.x += (*image)[position + width * 3] * (1 - dx) * (dy);
        TColor.y = (*image)[position + 1] * (1 - dx) * (1 - dy);
        TColor.y += (*image)[position + 3 + 1] * (dx) * (1 - dy);
        TColor.y += (*image)[position + 3 + width * 3 + 1] * (dx) * (dy);
        TColor.y += (*image)[position + width * 3 + 1] * (1 - dx) * (dy);
        TColor.z = (*image)[position + 2] * (1 - dx) * (1 - dy);
        TColor.z += (*image)[position + 3 + 2] * (dx) * (1 - dy);
        TColor.z += (*image)[position + 3 + width * 3 + 2] * (dx) * (dy);
        TColor.z += (*image)[position + width * 3 + 2] * (1 - dx) * (dy);
    }
    
    if(texture.decalMode == "replace_kd" || texture.decalMode == "blend_kd"){
        TColor.x /= 255.;
        TColor.y /= 255.;
        TColor.z /= 255.;
    }
    
    if (texture.decalMode == "blend_kd"){
        
        TColor = MatOp::vectorDivision(MatOp::vectorAddition(TColor, kd), 2.);
    }
    return TColor;
    
}



parser::Vec3f ColorCalculator::computeColor(parser::Ray ray, IntersectionCalculator::IntersectionData intersection, int recursionNumber,parser::Scene *scene, IntersectionCalculator &ic, vector<parser::TextureObject> textureObjects){
    
    
    parser::Vec3f pixelColor = {};
    int materialID = intersection.materialId;
    int textureid;
    if(textureObjects.size() < 1){
        textureid = -1;
    }else{
        textureid = intersection.texture_ID;
    }
    
    parser::Vec3f kd = scenePTR->materials[materialID-1].diffuse;
    
    parser::Vec3f TexelColorConstant = CalculateTCC(textureid, intersection, kd, textureObjects );
    
    bool isReplaceAll;
    if(textureid == -1 ){
        isReplaceAll = false;
    }else{
        isReplaceAll = (scene->textures[textureid-1].decalMode == "replace_all");
    }
    
    
    parser::Vec3f point = MatOp::vectorAddition(ray.a, MatOp::vectorMultiplication(ray.b, intersection.t)); // find point on the object.
    parser::Vec3f eyeVector = MatOp::vectorSubtraction(ray.a, point);
    parser::Vec3f normalizedEyeVector = MatOp::vectorNormalize(eyeVector);
    parser::Vec3f ambientReflectance = getAmbientReflectance(materialID);
    parser::Vec3f ambienLight = scenePTR->ambient_light;
    pixelColor.x = (ambienLight.x * ambientReflectance.x);
    pixelColor.y = (ambienLight.y * ambientReflectance.y);
    pixelColor.z = (ambienLight.z * ambientReflectance.z);
    
    
    
    
        
        //pixelColor = TexelColorConstant;
       // if not replace all

    
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
            parser::Vec3f diffuseContribution;
            
            if(isReplaceAll){
                diffuseContribution =  TexelColorConstant;

                
            }else{
                diffuseContribution = computeDiffuse(TexelColorConstant, intersection.normal, normalizedLightDirection, irradiance );
            }
            
            
            pixelColor = MatOp::vectorAddition(diffuseContribution, pixelColor); // add diffuse contribution to the pixel color
            
            // compute specular contribution
            parser::Vec3f normalizedHalfVector = MatOp::vectorNormalize(MatOp::vectorAddition(normalizedLightDirection, normalizedEyeVector));
            parser::Vec3f specularContribution = computeSpecular(materialID, intersection.normal, irradiance, normalizedHalfVector );
            pixelColor = MatOp::vectorAddition(specularContribution, pixelColor); // add specular contribution to the pixel color
        }
        else if(isReplaceAll){
            parser::Vec3f diffuseContribution;
            diffuseContribution =  TexelColorConstant;
            pixelColor = MatOp::vectorAddition(diffuseContribution, pixelColor); // add diffuse contribution to the pixel color
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
            parser::Vec3f reflectedRadiance = computeColor(reflectedRay, reflectedIntersection, recursionNumber-1, scene, ic, textureObjects);
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
