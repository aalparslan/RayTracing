#include <iostream>
#include "parser.hpp"
#include "Transformator.hpp"
#include "MathematicalOperations.hpp"
#include <stdexcept>
#include <bits/stdc++.h>
#include <cmath>
#include <stdio.h>

// Init 4x4 matrix
Transformator::Transformator() :
    floating_precision(0.001)
    {
    // Init transformation matrix
    this->transformation_matrix = createMatrix(4, 4);

    transformation_matrix[0][0] = 1;
    transformation_matrix[1][1] = 1;
    transformation_matrix[2][2] = 1;
    transformation_matrix[3][3] = 1;
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
    Transformator::printMatrix(this->transformation_matrix);
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
        transformed_points[column].x = input_matrix[0][column];
        transformed_points[column].y = input_matrix[1][column];
        transformed_points[column].z = input_matrix[2][column];

        // std::cout << input_points[column].x << ' ' << transformed_points[column].x << std::endl; // TODO - burada bug var!
    }


    return transformed_points;

}


// Rotate around pivot vector, angle in degrees
void Transformator::rotate(parser::Vec3f u, float angle){
    /*
     * Using alternative method in the slides
     * 1) Form v and w vectors from given u(pivot_vector)
     * 2) Form the rotation matrix to match axis
     * 3) Use axis-rotation matrices to rotate
     * 4) Multiply all
     */
    // 1)
    parser::Vec3f v;
    parser::Vec3f w;
    
    // Form the first perpendicular vector
    if(std::abs(u.x) < this->floating_precision && std::abs(u.y) < this->floating_precision){
        // Avoid vanishing x-y parameters
        v.x = 1;
        v.y = 0;
        v.z = 0;
    }else{
        v.x =  u.y;
        v.y = -u.x;
        v.z = 0;
    }
    
    // Form the second perpendicular vector
    w = MatOp::vectorCrossProduct(u, v);

    // Normalize vectors
    u = MatOp::vectorNormalize(u);
    v = MatOp::vectorNormalize(v);
    w = MatOp::vectorNormalize(w);
    
    // 2)
    Transformator::Matrix M = createMatrix(4, 4);

    M[0][0] = u.x;
    M[0][1] = u.y;
    M[0][2] = u.z;
    M[0][3] = 0;

    M[1][0] = v.x;
    M[1][1] = v.y;
    M[1][2] = v.z;
    M[1][3] = 0;

    M[2][0] = w.x;
    M[2][1] = w.y;
    M[2][2] = w.z;
    M[2][3] = 0;

    M[3][0] = 0;
    M[3][1] = 0;
    M[3][2] = 0;
    M[3][3] = 1;

    // Transformator::printMatrix(M);
    
    Transformator::Matrix inv_M = createMatrix(4, 4);
    
    inv_M[0][0] = u.x;
    inv_M[0][1] = v.x;
    inv_M[0][2] = w.x;
    inv_M[0][3] = 0;

    inv_M[1][0] = u.y;
    inv_M[1][1] = v.y;
    inv_M[1][2] = w.y;
    inv_M[1][3] = 0;

    inv_M[2][0] = u.z;
    inv_M[2][1] = v.z;
    inv_M[2][2] = w.z;
    inv_M[2][3] = 0;

    inv_M[3][0] = 0;
    inv_M[3][1] = 0;
    inv_M[3][2] = 0;
    inv_M[3][3] = 1;
    
    // 3)
    Transformator::Matrix R = createMatrix(4, 4);

    // Covert degrees to radian
    angle = angle / 180 * M_PI;

    // Form matrix
    R[0][0] = 1;

    R[1][1] =   std::cos(angle);
    R[1][2] = - std::sin(angle);
    R[2][1] =   std::sin(angle);
    R[2][2] =   std::cos(angle);

    R[3][3] = 1;

    // 4)
    // Multiply the object matrix to stack it
    Transformator::Matrix rotation_matrix;

    rotation_matrix = matmul(R, M);
    rotation_matrix = matmul(inv_M, rotation_matrix);

    this->transformation_matrix = matmul(rotation_matrix, this->transformation_matrix);
}

// Direct scale
void Transformator::scale(float x_scale, float y_scale, float z_scale){
    Transformator::Matrix translation_matrix = createMatrix(4, 4);

    // Form matrix
    translation_matrix[0][0] = x_scale;
    translation_matrix[1][1] = y_scale;
    translation_matrix[2][2] = z_scale;
    translation_matrix[3][3] = 1;

    // Multiply the object matrix to stack it
    this->transformation_matrix = matmul(translation_matrix, this->transformation_matrix);
}


// Translation
void Transformator::translate(parser::Vec3f offset){
    Transformator::Matrix translation_matrix = createMatrix(4, 4);
    
    // std::cout << offset.x << std::endl;
    
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


void Transformator::printMatrix(const Transformator::Matrix &W){
	int M = W.size();
	int N = W[0].size();

	printf("      ");
	for(int n = 0; n < N; n++){
		printf("%7d", n);
	}
	printf("\n");
	printf("      ");
	for(int n = 0; n < N+2; n++){
		for(int x = 0 ; x < 7; x++){
            printf("_");
        }
	}
	printf("\n");

	for(int m = 0; m < M; m++){
		printf("%7d", m);
		printf("  |");
		for(int n = 0; n < N; n++){
			printf("%7.2lf", W[m][n]);
		}
		printf("\n");
	}

}