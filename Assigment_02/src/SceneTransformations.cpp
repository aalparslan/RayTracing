#include "SceneTransformations.hpp"

void SceneTransformations::applyTriangleModelTransformations(parser::Scene &scene){
    for(int triangle_index = 0; triangle_index < scene.triangles.size(); triangle_index++){
        std::string transformations = scene.triangles[triangle_index].transformations;

        // Find the transformation
        std::vector<std::pair<char, char>> op_codes = parseString(transformations);

        // Calculate the vertices
        std::vector<parser::Vec3f> triangle_vertices;
        triangle_vertices.push_back(scene.vertex_data[scene.triangles[triangle_index].indices.v0_id-1]);
        triangle_vertices.push_back(scene.vertex_data[scene.triangles[triangle_index].indices.v1_id-1]);
        triangle_vertices.push_back(scene.vertex_data[scene.triangles[triangle_index].indices.v2_id-1]);


        // Apply it!
        // std::cout << triangle_vertices[0].x << std::endl;
        decideDoOperation(op_codes, scene, triangle_vertices);
        // std::cout << triangle_vertices[0].x << std::endl;

        // Add new vertices to the end and fix the points
        scene.vertex_data.push_back(triangle_vertices[0]);
        scene.triangles[triangle_index].indices.v0_id = scene.vertex_data.size();
        scene.vertex_data.push_back(triangle_vertices[1]);
        scene.triangles[triangle_index].indices.v1_id = scene.vertex_data.size();
        scene.vertex_data.push_back(triangle_vertices[2]);
        scene.triangles[triangle_index].indices.v2_id = scene.vertex_data.size();

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
void SceneTransformations::decideDoOperation(std::vector<std::pair<char, char>> operation_codes, const parser::Scene &scene, std::vector<parser::Vec3f> &target_vertices){
    if(operation_codes.size() == 0){
        return;
    }
    
    Transformator t;

    for(int operation_num = 0; operation_num < operation_codes.size(); operation_num++){
        
        int transformation_index = operation_codes[operation_num].second - '0';
        // Be sure indexes start with 1!
        transformation_index = transformation_index - 1;

        switch(operation_codes[operation_num].first){
            case 's':
                // TODO - scale
                break;

            case 'r':
                // TODO - rotate
                break;

            case 't':
                // TODO - xyz ekseninde bir sikinti var
                t.translate(scene.translations[transformation_index]);
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
}
