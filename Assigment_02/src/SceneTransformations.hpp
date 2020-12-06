#ifndef _SCENE_TRANSFORMATIONS
#define _SCENE_TRANSFORMATIONS

#include <iostream>
#include "parser.hpp"


class SceneTransformations{
    /*
     * This is a static class and aim is to remove burden on main.cpp
     * Call the methods directly, this will handle Transformator.
     * 
     * WARNING: THIS WILL CHANGE THE SCENE DATA!!!
     */

    public:
    static void applyTriangleModelTransformations(parser::Scene &scene);
    static void applyMeshModelTransformations(parser::Scene &scene);
    static void applySphereModelTransformations(parser::Scene &scene);

    private:
    static std::vector<std::pair<char, char>> parseString(std::string str);
    static double decideDoOperation(std::vector<std::pair<char, char>> operation_codes, const parser::Scene &scene, std::vector<parser::Vec3f> &target_vertices);
};


#endif