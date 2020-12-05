/*
Helper functions for algebra etc.
*/
#ifndef HW1_HELPER
#define HW1_HELPER

#include <iostream>
#include "parser.hpp"
#include "ppm.hpp"
#include <math.h>



parser::Vec3f vectorDifference(parser::Vec3f a, parser::Vec3f b); // Calculate a - b
float vectorLength(parser::Vec3f v); // Calculate vector length
float dot(parser::Vec3f a, parser::Vec3f b); // Calculate dot product
parser::Vec3f vectorDivision(parser::Vec3f v, float div); // Divide vector by a scalar
parser::Vec3f vectorMultiplication(parser::Vec3f v, float mul); // Multiply vector by a scalar
parser::Vec3f vectorAddition(parser::Vec3f v, parser::Vec3f u); // Vectoral summation
parser::Vec3f vectorCrossProduct(parser::Vec3f v, parser::Vec3f u); // Get a perpendicular vector
parser::Vec3f vectorNormalize(parser::Vec3f v); // Divide vector by its length

#endif