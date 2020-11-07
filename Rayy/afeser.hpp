#ifndef _HW1_AFESER
#define _HW1_AFESER

#include "helper.hpp"


/*
Minimum and maximum points of a mesh are stored inside two 3 dimensional vectors.
*/
typedef struct MinimumMaximumPoints{
    parser::Vec3f minimum;
    parser::Vec3f maximum;
} MinimumMaximumPoints;

/*
Precompute normal vectors and store in a vector.

    simpleTriangleNormalVectors : stores the normal data for each simple triangle.
    meshNormalVectors           : stores a vector that stores every normal vector for a face of each triangle in a mesh.
    meshMinMaxPoints            : stores the 3-dimensional max and min x,y,z coordinates
    
These two will give the normal vector:
    meshNormalVectors[nth_triangleFace]
    meshNormalVectors[nth_mesh][mth_triangleFace]
*/

typedef struct PrecomputedVariables{
    std::vector<parser::Vec3f>                                         *simpleTriangleNormalVectors;
    std::vector<std::vector<parser::Vec3f>*>                           *meshNormalVectors;
    std::vector<MinimumMaximumPoints>                                  *meshMinMaxPoints;
} PrecomputedVariables;



extern PrecomputedVariables precomputedVariables;

void precomputeAllVariables(const parser::Scene &scene); // Precompute all possible variables

void freeNormalVectorMemory(); // Free memory at the end

float  intersectTriangle(
    const parser::Ray &ray, 
    const parser::Face &face,
    std::vector<parser::Vec3f> &vertexData
);

std::pair<int, float> intersectMesh(
    const parser::Ray &ray,
    const std::vector<parser::Face> &faces,
    std::vector<parser::Vec3f> &vertexData,
    const float tThreshold,                 // Threshold to determine if the vertex is close than others
    const float naturalThreshold,           // Threshold to determine if a vertex intersection is really an intersection in 3D world(-1 output etc.)
    const int meshIndex                     // Index of the mesh for precomputed variables
);

#endif