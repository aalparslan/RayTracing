

#ifndef _COLOR_CALCULATOR
#define _COLOR_CALCULATOR
#include "parser.hpp"
#include "IntersectionCalculator.hpp"


class ColorCalculator{
public:
    ColorCalculator(parser::Scene *scene);
    ~ColorCalculator();
    
    parser::Vec3f computeColor(parser::Ray ray, IntersectionCalculator::IntersectionData intersection, int recursionNumber,parser::Scene *scene, IntersectionCalculator &ic, std::vector<parser::TextureObject> textureObjects);
    
private:
    // Pointer storage
    parser::Vec3f *vertexData_PTR;
    parser::Scene *scenePTR;
    
    int numberOfSpheres;
    int numberOfTriangles;
    int numberOfMeshes;
    
    
    parser::Sphere *spheres;
    parser::Triangle *triangles;
    parser::Mesh  *meshes;
    
    
    
    parser::Vec3f CalculateTextureConstant(int textureID,
                                        IntersectionCalculator::IntersectionData intersection,
                                        parser::Vec3f kd,
                                        std::vector<parser::TextureObject> textureObjects);
    parser::Vec3f getAmbientReflectance(int materialID);
    parser::Vec3f getDiffuseReflectance(int materialID);
    parser::Vec3f getspecularReflectance(int materialID);
    
    float getPhongExponent(int materialID);
    
    parser::Vec3f computeLightContribution(parser::Vec3f point, parser::PointLight light);
    parser::Vec3f computeDiffuse(parser::Vec3f texelConstant, parser::Vec3f normal, parser::Vec3f normalizedLightDirection, parser::Vec3f irradiance);
    parser::Vec3f computeSpecular(int materialID, parser::Vec3f normal, parser::Vec3f irradiance, parser::Vec3f normalizedHalfVector );
    
    void loadScene(parser::Scene * scene);
    
};

#endif
