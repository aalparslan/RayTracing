#include <iostream>
#include "parser.hpp"
#include "Transformator.hpp"
#include <stdexcept>
#include <bits/stdc++.h>


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


// Matrix multiplication
Transformator::Matrix Transformator::matmul(const Transformator::Matrix &m1, const Transformator::Matrix &m2) const{
    int m1_rows = m1.size();
    int m2_rows = m2.size();

    int m1_cols = m1[0].size();
    int m2_cols = m2[0].size();

    if(m1_cols != m2_rows){
        throw std::invalid_argument("Invalid Matrix sizes in matmul");
    }

    Transformator::Matrix result_matrix = createMatrix(m1_rows, m2_cols);

    for(int m1_row_counter = 0; m1_row_counter < m1_rows; m1_row_counter++){
        for(int m2_column_counter = 0; m2_column_counter < m2_cols; m2_column_counter++){
            // Do for each element
            double value = 0;
            for(int m1_column_counter = 0; m1_column_counter < m1_cols; m1_column_counter++){
                value += m1[m1_row_counter][m1_column_counter] * m2[m1_column_counter][m2_column_counter];
            }
            result_matrix[m1_row_counter][m2_column_counter] = value;

        }
        
    }

    return result_matrix;
    
}

// Create matrix from the given dimensions
Transformator::Matrix Transformator::createMatrix(int rows, int columns) const{
    Transformator::Matrix matrix;

    matrix = std::vector<std::vector<double>>(rows);

    for(int x = 0; x < rows; x++){
        matrix[x] = std::vector<double>(columns);
    }

    return matrix;
}

// Compute the stacked transformations
std::vector<parser::Vec3f> Transformator::transformPoints(const std::vector<parser::Vec3f> &input_points){
    /*
     * 1) Convert vector coordiantes to a matrix
     * 2) Do matrix multiplication
     * 3) Convert the result matrix to vector coordinates
     */
    // 1)
    Transformator::Matrix input_matrix = createMatrix(4, input_points.size());

    // Map input to the matrix
    for(int column = 0; column < input_matrix[0].size(); column++){
        input_matrix[0][column] = input_points[column].x;
        input_matrix[1][column] = input_points[column].y;
        input_matrix[2][column] = input_points[column].z;
        input_matrix[3][column] = 1;
    }

    // 2)
    input_matrix = matmul(this->transformation_matrix, input_matrix);

    // 3)
    std::vector<parser::Vec3f> transformed_points(input_matrix[0].size());
    for(int column = 0; column < input_matrix[0].size(); column++){
        transformed_points[column].x = input_matrix[column][0];
        transformed_points[column].y = input_matrix[column][1];
        transformed_points[column].z = input_matrix[column][2];

        // std::cout << input_points[column].x << ' ' << transformed_points[column].x << std::endl; // TODO - burada bug var!
    }


    return transformed_points;

}

// Init 4x4 matrix
Transformator::Transformator(){
    // Init transformation matrix
    this->transformation_matrix = createMatrix(4, 4);

    transformation_matrix[0][0] = 1;
    transformation_matrix[1][1] = 1;
    transformation_matrix[2][2] = 1;
    transformation_matrix[3][3] = 1;
}

// Direct rotate
void Transformator::rotate(double angle){
    // TODO
}
// Rotate around pivot vector, angle in degrees
void Transformator::rotate(parser::Vec3f pivot_vector_start, parser::Vec3f pivot_vector_end, double angle){
    // TODO
}

// Direct scale
void Transformator::scale(double x_scale, double y_scale, double z_scale){
    // TODO
}
// Scale around pivot
void Transformator::scale(double x_scale, double y_scale, double z_scale, parser::Vec3f pivot_point){
    // TODO
}

// Translation
void Transformator::translate(parser::Vec3f offset){
    Transformator::Matrix translation_matrix = createMatrix(4, 4);
    
    std::cout << offset.x << std::endl;
    
    // Form matrix
    translation_matrix[0][0] = 1;
    translation_matrix[1][1] = 1;
    translation_matrix[2][2] = 1;
    translation_matrix[3][3] = 1;

    translation_matrix[0][3] = offset.x;
    translation_matrix[1][3] = offset.y;
    translation_matrix[2][3] = offset.z;


    // Multiply the object matrix to stack it
    this->transformation_matrix = matmul(translation_matrix, this->transformation_matrix);

}

