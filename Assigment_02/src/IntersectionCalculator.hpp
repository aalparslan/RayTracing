
#ifndef _INTERSECTIONCALCULATOR
#define _INTERSECTIONCALCULATOR

#include "helper.hpp"
#include "parser.hpp"

class IntersectionCalculator{
    public:
    // Constructor...
    IntersectionCalculator(parser::Scene scene);
    // Destructor
    ~IntersectionCalculator();

    // This function does all the job actually. It computes
    // the closest object normal vector by looking all possible
    // scenarious
    typedef struct IntersectionData{
        float t;
        parser::Vec3f normal;
        int materialId; 
    } IntersectionData;
    IntersectionData intersectRay(parser::Ray ray, float treshold) const;

    float intersectSphere(parser::Ray ray, parser::Sphere sphere) const;
    
    float intersectTriangle(
        const parser::Ray &ray, 
        const parser::Face &face
    ) const;

    std::pair<int, float> intersectMesh(
        const parser::Ray &ray,
        const std::vector<parser::Face> &faces,
        const std::vector<parser::Vec3f> &vertexData,
        const float tThreshold,                 // Threshold to determine if the vertex is close than others
        const float naturalThreshold,           // Threshold to determine if a vertex intersection is really an intersection in 3D world(-1 output etc.)
        const int meshIndex                     // Index of the mesh for precomputed variables
    ) const;
    

    private:
    // Fiels
        const parser::Scene scene;
    // Precomputation
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

        
        PrecomputedVariables precomputedVariables;
        void precomputeMinMaxCoordinates(const parser::Scene &scene);
        void precomputeNormalVectors(const parser::Scene &scene);
        void precomputeAllVariables(const parser::Scene &scene); // Precompute all possible variables

        void freeNormalVectorMemory(); // Free memory at the end

    // Helper private methods
    bool intersectBox(const parser::Ray &ray, const MinimumMaximumPoints &minimumMaximumPoints) const;
    parser::Vec3f computeNormalVector(const parser::Face &face, const std::vector<parser::Vec3f> &vertex_data) const;
};

#endif