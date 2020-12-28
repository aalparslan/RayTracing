
#include "parser.hpp"
#include "MathematicalOperations.hpp"

parser::Vec3f MatOp::vectorSubtraction(parser::Vec3f a, parser::Vec3f b){
    
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    
    return a;
}
float MatOp::dot(parser::Vec3f a, parser::Vec3f b){
    /*
    Calculate dot product of a and b.
    */
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
parser::Vec3f MatOp::vectorDifference(parser::Vec3f a, parser::Vec3f b){
    /*
    Calculate a - b.
    */
   return parser::Vec3f{a.x - b.x, a.y - b.y, a.z - b.z};
}
float MatOp::vectorLength(parser::Vec3f v){
    /*
    Calculate vector length.
    */
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
parser::Vec3f MatOp::vectorDivision(parser::Vec3f v, float div){
    v.x = v.x / div;
    v.y = v.y / div;
    v.z = v.z / div;
    
    return v;
}
parser::Vec3f MatOp::vectorMultiplication(parser::Vec3f v, float mul){
    v.x = v.x * mul;
    v.y = v.y * mul;
    v.z = v.z * mul;
    
    return v;
}
parser::Vec3f MatOp::vectorAddition(parser::Vec3f v, parser::Vec3f u){
    v.x = v.x + u.x;
    v.y = v.y + u.y;
    v.z = v.z + u.z;
    
    return v;
}
parser::Vec3f MatOp::vectorCrossProduct(parser::Vec3f a, parser::Vec3f b){
    /*
    Calculates w = a x b
    */
    parser::Vec3f w;
    w.x = a.y*b.z - b.y*a.z;
    w.y = b.x*a.z - a.x*b.z;
    w.z = a.x*b.y - b.x*a.y;
    
    return w;
}
parser::Vec3f MatOp::vectorNormalize(parser::Vec3f v){
    return vectorDivision(v, vectorLength(v));
}

