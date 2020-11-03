#ifndef HW1_AFESER
#define HW1_AFESER

#include "helper.hpp"

std::pair<double, parser::Vec3f>  intersectTriangle(const parser::Ray &ray, const parser::Triangle& triangle, std::vector<parser::Vec3f> &vertexData);

#endif