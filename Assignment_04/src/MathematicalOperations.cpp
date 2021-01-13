#include "parser.h"
#include "MathematicalOperations.hpp"


parser::Vec3f MatOp::computeNormalVector(const parser::Face &face, const std::vector<parser::Vec3f> &vertex_data){
    // Triangle constants...
    // Notice the indices start from 1, so decrement is needed
    float x_a = vertex_data[face.v0_id-1].x;
    float y_a = vertex_data[face.v0_id-1].y;
    float z_a = vertex_data[face.v0_id-1].z;

    float x_b = vertex_data[face.v1_id-1].x;
    float y_b = vertex_data[face.v1_id-1].y;
    float z_b = vertex_data[face.v1_id-1].z;

    float x_c = vertex_data[face.v2_id-1].x;
    float y_c = vertex_data[face.v2_id-1].y;
    float z_c = vertex_data[face.v2_id-1].z;
    // Calculate the edge vectors
    // Direction is from second vertex to first and third
    parser::Vec3f edge1, edge2, normal;

    edge1.x = x_a - x_b;
    edge1.y = y_a - y_b;
    edge1.z = z_a - z_b;

    edge2.x = x_c - x_b;
    edge2.y = y_c - y_b;
    edge2.z = z_c - z_b;


    normal = MatOp::vectorNormalize(MatOp::vectorCrossProduct(edge2, edge1));

    return normal;

}
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
