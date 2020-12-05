#include <iostream>
#include "parser.hpp"
#include "Transformator.hpp"
#include <stdexcept>

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
    std::vector<parser::Vec3f> transformed_points;
    for(int column = 0; column < input_matrix[0].size(); column++){
        transformed_points[column].x = input_matrix[column][0];
        transformed_points[column].y = input_matrix[column][1];
        transformed_points[column].z = input_matrix[column][2];
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

