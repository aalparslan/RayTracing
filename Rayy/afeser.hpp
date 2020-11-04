#ifndef _HW1_AFESER
#define _HW1_AFESER

#include "helper.hpp"

/*
Precompute normal vectors and store in a vector.

    simpleTriangleNormalVectors : stores the normal data for each simple triangle.
    meshNormalVectors           : stores a vector that stores every normal vector for a face of each triangle in a mesh.

These two will give the normal vector:
    meshNormalVectors[nth_triangleFace]
    meshNormalVectors[nth_mesh][mth_triangleFace]
*/
typedef struct PrecomputeNormalVectors{
    std::vector<parser::Vec3f>               *simpleTriangleNormalVectors;
    std::vector<std::vector<parser::Vec3f>*> *meshNormalVectors;
} PrecomputedNormalVectors;

extern PrecomputedNormalVectors precomputedNormalVectors;

void precomputeNormalVectors(const parser::Scene &scene); // Precompute the data structure above
void freeNormalVectorMemory(); // Free memory at the end

double  intersectTriangle(
    const parser::Ray &ray, 
    const parser::Face &face,
    std::vector<parser::Vec3f> &vertexData
);

std::vector<double>  intersectMesh(
    const parser::Ray &ray,
    const std::vector<parser::Face> &faces,
    std::vector<parser::Vec3f> &vertexData
);

#endif