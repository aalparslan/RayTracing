
#ifndef _MATHEMATICALOPERATIONS
#define _MATHEMATICALOPERATIONS
#include "parser.hpp"
#include <iostream>
#include "ppm.hpp"
#include <math.h>

class MatOp{
    public:
    static parser::Vec3f vectorDifference(parser::Vec3f a, parser::Vec3f b); // Calculate a - b
    static float vectorLength(parser::Vec3f v); // Calculate vector length
    static float dot(parser::Vec3f a, parser::Vec3f b); // Calculate dot product
    static parser::Vec3f vectorDivision(parser::Vec3f v, float div); // Divide vector by a scalar
    static parser::Vec3f vectorMultiplication(parser::Vec3f v, float mul); // Multiply vector by a scalar
    static parser::Vec3f vectorAddition(parser::Vec3f v, parser::Vec3f u); // Vectoral summation
    static parser::Vec3f vectorCrossProduct(parser::Vec3f v, parser::Vec3f u); // Get a perpendicular vector
    static parser::Vec3f vectorNormalize(parser::Vec3f v); // Divide vector by its length

};

#endif