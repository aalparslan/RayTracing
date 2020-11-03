/*
Helper functions for algebra etc.
*/
#ifndef HW1_HELPER
#define HW1_HELPER

#include <iostream>
#include "parser.hpp"
#include "ppm.hpp"
#include <math.h>


typedef unsigned char MonoPixel; // Pixel data type
struct Color{ // Alparslan - RGB array oldugu icin return type icinde sikinti cikardi, bunu olusturdum bu yuzden
    MonoPixel R;
    MonoPixel G;
    MonoPixel B;
};


parser::Vec3f vectorDifference(parser::Vec3f a, parser::Vec3f b); // Calculate a - b
double vectorLength(parser::Vec3f v); // Calculate vector length
double dot(parser::Vec3f a, parser::Vec3f b); // Calculate dot product
parser::Vec3f vectorDivision(parser::Vec3f v, double div); // Divide vector by a scalar
parser::Vec3f vectorMultiplication(parser::Vec3f v, double mul); // Multiply vector by a scalar
parser::Vec3f vectorAddition(parser::Vec3f v, parser::Vec3f u); // Vectoral summation

#endif HW1_HELPER