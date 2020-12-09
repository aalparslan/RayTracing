#include "SceneTransformations.hpp"
#include "Transformator.hpp"
#include "MathematicalOperations.hpp"
#include <bits/stdc++.h>
#include <stdexcept>


void SceneTransformations::saveTextureFaceIds(parser::Scene &scene){
    // Triangles
    for(int triangle_index = 0; triangle_index < scene.triangles.size(); triangle_index++){
        scene.triangles[triangle_index].texture_indices = scene.triangles[triangle_index].indices;
    }

    // Meshes
    for(int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++){
        scene.meshes[mesh_index].texture_faces = std::vector<parser::Face>();
        for(int face_index= 0; face_index < scene.meshes[mesh_index].faces.size(); face_index++){
            scene.meshes[mesh_index].texture_faces.push_back(scene.meshes[mesh_index].faces[face_index]);
        }
    }


}
void SceneTransformations::applySphereModelTransformations(parser::Scene &scene){
    for(int sphere_index = 0; sphere_index < scene.spheres.size(); sphere_index++){
        parser::Sphere &sphere = scene.spheres[sphere_index];
        std::string transformations = scene.spheres[sphere_index].transformations;

        // Find the transformation
        std::vector<std::pair<char, char>> op_codes = parseString(transformations);


        std::vector<parser::Vec3f> sphere_center(1);
        std::vector<parser::Vec3f> texture_vector(1);
        
        // Calculate the center and texture vector
        sphere_center[0] = scene.vertex_data[sphere.center_vertex_id-1];
        texture_vector[0] = parser::Vec3f({1., 1., 0.});
        
        // Apply it!
        double raidus_mult = decideDoOperation(op_codes, scene, sphere_center, false);
        decideDoOperation(op_codes, scene, texture_vector, true);

        // Map back to the sphere
        scene.vertex_data.push_back(sphere_center[0]);
        sphere.center_vertex_id = scene.vertex_data.size();
        sphere.base_texture_vector = MatOp::vectorNormalize(texture_vector[0]);

        // Do not forget to update radius!
        sphere.radius = sphere.radius * raidus_mult;

    }
}
void SceneTransformations::applyMeshModelTransformations(parser::Scene &scene){
    for(int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++){
        std::vector<parser::Face> &faces = scene.meshes[mesh_index].faces;
        std::string transformations = scene.meshes[mesh_index].transformations;

        // Find the transformation
        std::vector<std::pair<char, char>> op_codes = parseString(transformations);


        std::vector<parser::Vec3f> triangle_vertices(faces.size()*3);
            
        // Apply to each face!
        int counter1 = 0;
        for(int face_index = 0; face_index < faces.size(); face_index++){
            parser::Face &face = faces[face_index];

            // Calculate the vertices            
            triangle_vertices[counter1++] = (scene.vertex_data[face.v0_id-1]);
            triangle_vertices[counter1++] = (scene.vertex_data[face.v1_id-1]);
            triangle_vertices[counter1++] = (scene.vertex_data[face.v2_id-1]);

        }

        // Apply it!
        decideDoOperation(op_codes, scene, triangle_vertices, false);

        // Map back to the faces
        int counter2 = 0;
        for(int face_index = 0; face_index < faces.size(); face_index++){
            parser::Face &face = faces[face_index];
            
            // Add new vertices to the end and fix the points
            scene.vertex_data.push_back(triangle_vertices[counter2++]);
            face.v0_id = scene.vertex_data.size();
            scene.vertex_data.push_back(triangle_vertices[counter2++]);
            face.v1_id = scene.vertex_data.size();
            scene.vertex_data.push_back(triangle_vertices[counter2++]);
            face.v2_id = scene.vertex_data.size();
        }


    }
}
void SceneTransformations::applyTriangleModelTransformations(parser::Scene &scene){
    for(int triangle_index = 0; triangle_index < scene.triangles.size(); triangle_index++){
        parser::Face &face = scene.triangles[triangle_index].indices;
        std::string transformations = scene.triangles[triangle_index].transformations;

        // Find the transformation
        std::vector<std::pair<char, char>> op_codes = parseString(transformations);

        // Calculate the vertices
        std::vector<parser::Vec3f> triangle_vertices;
        triangle_vertices.push_back(scene.vertex_data[face.v0_id-1]);
        triangle_vertices.push_back(scene.vertex_data[face.v1_id-1]);
        triangle_vertices.push_back(scene.vertex_data[face.v2_id-1]);


        // Apply it!
        // std::cout << triangle_vertices[0].x << std::endl;
        decideDoOperation(op_codes, scene, triangle_vertices, false);
        // std::cout << triangle_vertices[0].x << std::endl;

        // Add new vertices to the end and fix the points
        scene.vertex_data.push_back(triangle_vertices[0]);
        face.v0_id = scene.vertex_data.size();
        scene.vertex_data.push_back(triangle_vertices[1]);
        face.v1_id = scene.vertex_data.size();
        scene.vertex_data.push_back(triangle_vertices[2]);
        face.v2_id = scene.vertex_data.size();

    }
}
std::vector<std::pair<char, char>> SceneTransformations::parseString(std::string str){
    std::istringstream stream(str);

    std::vector<std::pair<char, char>> all_codes;


    std::string code;
    while(stream >> code){
        all_codes.push_back(std::pair<char, char>(code[0], code[1]));
    }

    return all_codes;
}
double SceneTransformations::decideDoOperation(std::vector<std::pair<char, char>> operation_codes, const parser::Scene &scene, std::vector<parser::Vec3f> &target_vertices, bool only_rotate){
    /*
     * Do operations and return the scaling factor.
     * This factor is computed on x axis scale component and useful for Sphere raidus calculations.
     */
    double scale_multiplier = 1.0;

    if(operation_codes.size() == 0){
        return 1.0;
    }
    
    Transformator t;

    for(int operation_num = 0; operation_num < operation_codes.size(); operation_num++){
        
        int transformation_index = operation_codes[operation_num].second - '0';
        // Be sure indices start with 1!
        transformation_index = transformation_index - 1;

        switch(operation_codes[operation_num].first){
            case 's': if(!only_rotate) {
                scale_multiplier = scale_multiplier * scene.scalings[transformation_index].x;
                t.scale(
                    scene.scalings[transformation_index].x,
                    scene.scalings[transformation_index].y,
                    scene.scalings[transformation_index].z
                );
                
            }
            break;

            case 'r': {
                parser::Vec3f rotate_around;
                rotate_around.x = scene.rotations[transformation_index].x;
                rotate_around.y = scene.rotations[transformation_index].y;
                rotate_around.z = scene.rotations[transformation_index].z;

                float angle = scene.rotations[transformation_index].angle;
                
                t.rotate(rotate_around, angle);
            }
            break;

            case 't': if(!only_rotate){
                t.translate(scene.translations[transformation_index]);
            }
            break;
            
            default:
                throw std::invalid_argument("Can't decide transformation operation, possibly parsing failed...");
        }


    }

    // After the transformation matrix is created
    // Simply calculate the result and write back!
    std::vector<parser::Vec3f> new_coords = t.transformPoints(target_vertices);

    for(int vertex_index = 0; vertex_index < target_vertices.size(); vertex_index++){
        target_vertices[vertex_index] = new_coords[vertex_index];
    }

    return scale_multiplier;
}
