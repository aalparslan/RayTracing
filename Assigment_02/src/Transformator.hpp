#ifndef _TRANSFORMATOR
#define _TRANSFORMATOR

#include <iostream>
#include "parser.hpp"

class SceneTransformations{
    /*
     * This is a static class and aim is to remove burden on main.cpp
     * Call the methods directly, this will handle Transformator.
     * 
     * WARNING: THIS WILL CHANGE THE SCENE DATA!!!
     */

    public:
    static void applyTriangleModelTransformations(parser::Scene &scene);

    private:
    static std::vector<std::pair<char, char>> parseString(std::string str);
    static void decideDoOperation(std::vector<std::pair<char, char>> operation_codes, const parser::Scene &scene, std::vector<parser::Vec3f> &target_vertices);
};


class Transformator{
    /*
     * The class is designed for lazy evaluation.
     * All matrix operations wait as long as result
     * is claimed explicitely.
     */

    public:
    
    // Compute the stacked transformations
    std::vector<parser::Vec3f> transformPoints(const std::vector<parser::Vec3f> &input_points);

    // Init 4x4 matrix
    Transformator();

    // Direct rotate
    void rotate(double angle);
    // Rotate around pivot vector, angle in degrees
    void rotate(parser::Vec3f pivot_vector_start, parser::Vec3f pivot_vector_end, double angle);

    // Direct scale
    void scale(double x_scale, double y_scale, double z_scale);
    // Scale around pivot
    void scale(double x_scale, double y_scale, double z_scale, parser::Vec3f pivot_point);

    // Translation
    void translate(parser::Vec3f offset);

    private:
    typedef std::vector<std::vector<double>> Matrix;
    Matrix transformation_matrix;

    // Matrix multiplication
    Matrix matmul(const Matrix &m1, const Matrix &m2) const;

    // Blank matrix
    Matrix createMatrix(int rows, int columns) const;
};

#endif