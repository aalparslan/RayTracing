#include "SceneTransformations.hpp"
#include "Transformator.hpp"
#include "MathematicalOperations.hpp"
#include <bits/stdc++.h>
#include <stdexcept>


parser::Vec3f SceneTransformations::rotateTextureVector(parser::Vec3f my_vec, std::string transformations, const parser::Scene &scene){
  std::vector<parser::Vec3f> process_vector(1);
  process_vector[0] = my_vec;
  std::vector<std::pair<char, int>> op_codes = SceneTransformations::parseString(transformations);
  SceneTransformations::decideDoOperation(op_codes, scene, process_vector, true);
  my_vec = process_vector[0];

  return my_vec;
}
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
        std::vector<std::pair<char, int>> op_codes = parseString(transformations);


        std::vector<parser::Vec3f> sphere_center(1);

        // Calculate the center
        sphere_center[0] = scene.vertex_data[sphere.center_vertex_id-1];

        // Apply it!
        double raidus_mult = decideDoOperation(op_codes, scene, sphere_center, false);

        // Map back to the sphere
        scene.vertex_data.push_back(sphere_center[0]);
        sphere.center_vertex_id = scene.vertex_data.size();

        // Do not forget to update radius!
        sphere.radius = sphere.radius * raidus_mult;


    }
}
void SceneTransformations::applyMeshModelTransformations(parser::Scene &scene){
    for(int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++){
        std::vector<parser::Face> &faces = scene.meshes[mesh_index].faces;
        std::string transformations = scene.meshes[mesh_index].transformations;

        // Find the transformation
        std::vector<std::pair<char, int>> op_codes = parseString(transformations);


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
        std::vector<std::pair<char, int>> op_codes = parseString(transformations);

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
std::vector<std::pair<char, int>> SceneTransformations::parseString(std::string str){
    std::istringstream stream(str);

    std::vector<std::pair<char, int>> all_codes;


    std::string code;
    while(stream >> code){
        int total = 0;

        // Go through the char digits...
        for(int counter = 1; counter < code.length(); counter++){
          int current_code = code[counter] - '0';

          if(current_code <= 9 && current_code >= 0){
            total += current_code * std::pow(10, code.length() - counter - 1);
          }

        }

        all_codes.push_back(std::pair<char, int>(code[0], total));
    }

    return all_codes;
}
double SceneTransformations::decideDoOperation(std::vector<std::pair<char, int>> operation_codes, const parser::Scene &scene, std::vector<parser::Vec3f> &target_vertices, bool only_texture_transformation){
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

        int transformation_index = operation_codes[operation_num].second;
        // Be sure indices start with 1!
        transformation_index = transformation_index - 1;

        switch(operation_codes[operation_num].first){
            case 's': if(!only_texture_transformation) {
                scale_multiplier = scale_multiplier * scene.scalings[transformation_index].x;
                t.scale(
                    scene.scalings[transformation_index].x,
                    scene.scalings[transformation_index].y,
                    scene.scalings[transformation_index].z
                );

            }
            break;

            case 'r': if(!only_texture_transformation){
                parser::Vec3f rotate_around;
                rotate_around.x = scene.rotations[transformation_index].x;
                rotate_around.y = scene.rotations[transformation_index].y;
                rotate_around.z = scene.rotations[transformation_index].z;

                float angle = scene.rotations[transformation_index].angle;

                t.rotate(rotate_around, angle);
            }else{
              parser::Vec3f rotate_around;
              rotate_around.x = scene.rotations[transformation_index].x;
              rotate_around.y = scene.rotations[transformation_index].y;
              rotate_around.z = scene.rotations[transformation_index].z;

              float angle = -scene.rotations[transformation_index].angle;

              t.rotate(rotate_around, angle);
            }
            break;

            case 't': if(!only_texture_transformation){
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
