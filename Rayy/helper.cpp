#include "helper.hpp"

double dot(parser::Vec3f a, parser::Vec3f b){
    /*
    Calculate dot product of a and b.
    */
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
parser::Vec3f vectorDifference(parser::Vec3f a, parser::Vec3f b){
    /*
    Calculate a - b.
    */
   return parser::Vec3f{a.x - b.x, a.y - b.y, a.z - b.z};
}
double vectorLength(parser::Vec3f v){
    /*
    Calculate vector length.
    */
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}


