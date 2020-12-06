#ifndef _TRANSFORMATOR
#define _TRANSFORMATOR

#include <iostream>
#include "parser.hpp"



class Transformator{
    /*
     * The class is designed for lazy evaluation.
     * All matrix operations wait as long as result
     * is claimed explicitely.
     */

    public:
    const double floating_precision;

    // Init 4x4 matrix
    Transformator();

    // Compute the stacked transformations
    std::vector<parser::Vec3f> transformPoints(const std::vector<parser::Vec3f> &input_points);

    // Rotate around pivot vector, angle in degrees
    void rotate(parser::Vec3f pivot_vector, double angle);

    // Direct scale
    void scale(double x_scale, double y_scale, double z_scale);
    
    // Translation
    void translate(parser::Vec3f offset);

    private:
    typedef std::vector<std::vector<double>> Matrix;
    Matrix transformation_matrix;

    // Matrix multiplication
    Matrix matmul(const Matrix &m1, const Matrix &m2) const;

    // Blank matrix
    Matrix createMatrix(int rows, int columns) const;

    // Print utility
    static void printMatrix(const Transformator::Matrix &W);

};

#endif