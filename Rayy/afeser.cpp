#include "helper.hpp"
#include "afeser.hpp"


parser::Vec3f computeNormalVector(const parser::Face &face, const std::vector<parser::Vec3f> &vertex_data){
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
    
    
    normal = vectorNormalize(vectorCrossProduct(edge2, edge1));

    return normal;

}
void precomputeNormalVectors(const parser::Scene &scene){
    // Allocate for simple triangles
    precomputedNormalVectors.simpleTriangleNormalVectors = new std::vector<parser::Vec3f>(scene.triangles.size());

    // Allocate for triangles inside a mesh
    precomputedNormalVectors.meshNormalVectors = new std::vector<std::vector<parser::Vec3f>*>(scene.meshes.size());

    for(int triangleCounter = 0; triangleCounter < scene.triangles.size(); triangleCounter++){
        auto &face        = scene.triangles[triangleCounter].indices;
        auto &vertex_data = scene.vertex_data;
        
        (*precomputedNormalVectors.simpleTriangleNormalVectors)[triangleCounter] = computeNormalVector(face, vertex_data);
    }

    // Precompute for meshes
    for(int meshCounter = 0; meshCounter < scene.meshes.size(); meshCounter++){
        // Iterate over triangles in each mesh
        (*precomputedNormalVectors.meshNormalVectors)[meshCounter] = new std::vector<parser::Vec3f>(scene.meshes[meshCounter].faces.size());

        for(int triangleCounter = 0; triangleCounter < scene.meshes[meshCounter].faces.size(); triangleCounter++){
            auto &face        = scene.meshes[meshCounter].faces[triangleCounter];
            auto &vertex_data = scene.vertex_data;

            (*(*precomputedNormalVectors.meshNormalVectors)[meshCounter])[triangleCounter] = computeNormalVector(face, vertex_data);
        }
    }
}
void freeNormalVectorMemory(){
    // Free triangle vector
    delete precomputedNormalVectors.simpleTriangleNormalVectors;
    
    // Free mesh vectors
    for(int meshCounter = 0; meshCounter < (*precomputedNormalVectors.meshNormalVectors).size(); meshCounter++){
        delete (*precomputedNormalVectors.meshNormalVectors)[meshCounter];
    }
    // Free also container of vectors
    delete precomputedNormalVectors.meshNormalVectors;
}

float intersectTriangle(const parser::Ray &ray, const parser::Face &face, std::vector<parser::Vec3f> &vertexData){
    /*
     Return t as float and normal vector as Vec3f.
     t = -1 if no intersection exists.

     (Snyder & Barr, 1987) method
     Textbook Page 78 notation used
     */
    // Ray constants...
    float x_e = ray.a.x;
    float y_e = ray.a.y;
    float z_e = ray.a.z;

    float x_d = ray.b.x;
    float y_d = ray.b.y;
    float z_d = ray.b.z;

    // Triangle constants...
    // Notice the indices start from 1, so decrement is needed
    float x_a = vertexData[face.v0_id-1].x;
    float y_a = vertexData[face.v0_id-1].y;
    float z_a = vertexData[face.v0_id-1].z;

    float x_b = vertexData[face.v1_id-1].x;
    float y_b = vertexData[face.v1_id-1].y;
    float z_b = vertexData[face.v1_id-1].z;

    float x_c = vertexData[face.v2_id-1].x;
    float y_c = vertexData[face.v2_id-1].y;
    float z_c = vertexData[face.v2_id-1].z;


    // Matrix elements...  
    float a = x_a - x_b;
    float b = y_a - y_b;
    float c = z_a - z_b;
    
    float d = x_a - x_c;
    float e = y_a - y_c;
    float f = z_a - z_c;

    float g = x_d;
    float h = y_d;
    float i = z_d;

    // Solution variables...
    float j = x_a - x_e;
    float k = y_a - y_e;
    float l = z_a - z_e;


    // Composite variables...
    float M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);


    // Main variables
    float beta  = j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g);
    float gamma = i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c);
    float t     = f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c);

    // Normalization...
    beta  = beta  / M;
    gamma = gamma / M;
    t     = t     / M * (-1);


    // TODO -> t_0 icin e noktasini, yani camera position aliyorum, bu yanlis olabilir mi?? cunku canvas ile kamera arasindaysa gormemeli sanirim? ya da gormeli mi :)
    if (
        t < 0     ||
        gamma < 0 || gamma + beta > 1 ||
        beta < 0
    ){
        // Will not use it anymore, return a null normal vector
        return -1;
    }else{
        return t;
    }


}
std::vector<float> intersectMesh(const parser::Ray &ray, const std::vector<parser::Face> &faces, std::vector<parser::Vec3f> &vertexData){
    /*
    This function is actually a set of combination for triangles.
    The structure implies there is a vector of faces that specify 3 vertex coordinates.
    
    Given implementation is straightforward, iterate 'intersectTriangle' and return a vector of pairs which include
    t value and surface normal.
    */
   // Return all normals and t values
   std::vector<float> allReturns(faces.size());

   // Do for each triangle...
   for(int counter = 0; counter < faces.size(); counter++){
       allReturns[counter] = intersectTriangle(ray, faces[counter], vertexData);
   }

   return allReturns;


}