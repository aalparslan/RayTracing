#ifndef HW1_AFESER
#define HW1_AFESER

#include "helper.hpp"

std::pair<double, parser::Vec3f>  intersectTriangle(
    const parser::Ray &ray, 
    const parser::Face &face,
    std::vector<parser::Vec3f> &vertexData
);

std::vector<std::pair<double, parser::Vec3f>>  intersectMesh(
    const parser::Ray &ray,
    const std::vector<parser::Face> &faces,
    std::vector<parser::Vec3f> &vertexData
);

#endif