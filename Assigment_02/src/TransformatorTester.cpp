#include <iostream>
#include "Transformator.hpp"
#include "parser.hpp"

using namespace std;

int translation_test(){
    Transformator t;

    parser::Vec3f a;
    a.x = 1;
    a.y = 2;
    a.z = 3;

    std::vector<parser::Vec3f> input;
    input.push_back(a);

    parser::Vec3f b;
    b.x = 3;
    b.y = 5;
    b.z = 3;
    
    t.translate(b);

    std::vector<parser::Vec3f> output = t.transformPoints(input);

    cout << input[0].x << " -> " << output[0].x << endl;
    cout << input[0].y << " -> " << output[0].y << endl;
    cout << input[0].z << " -> " << output[0].z << endl;
    
    return 0;
}
int scaling_test(){
    Transformator t;

    parser::Vec3f a;
    a.x = 1;
    a.y = 2;
    a.z = 3;

    std::vector<parser::Vec3f> input;
    input.push_back(a);
    
    t.scale(0.1, 0.2, 0.3);

    std::vector<parser::Vec3f> output = t.transformPoints(input);

    cout << input[0].x << " -> " << output[0].x << endl;
    cout << input[0].y << " -> " << output[0].y << endl;
    cout << input[0].z << " -> " << output[0].z << endl;
    
    return 0;
}
int rotation_test(){
    Transformator t;

    std::vector<parser::Vec3f> cube_coordinates;
    parser::Vec3f point;

    point = {10, 10,  0};
    cube_coordinates.push_back(point);
    point = {15, 10,  0};
    cube_coordinates.push_back(point);
    point = {15, 15,  0};
    cube_coordinates.push_back(point);
    point = {10, 15,  0};
    cube_coordinates.push_back(point);
    point = {10, 10, -5};
    cube_coordinates.push_back(point);
    point = {15, 10, -5};
    cube_coordinates.push_back(point);
    point = {15, 15, -5};
    cube_coordinates.push_back(point);
    point = {10, 15, -5};
    cube_coordinates.push_back(point);


    parser::Vec3f rotate_around;
    rotate_around.x = 0;
    rotate_around.y = 0;
    rotate_around.z = 2;
    double angle = 123;

    
    t.rotate(rotate_around, angle);

    std::vector<parser::Vec3f> output = t.transformPoints(cube_coordinates);

    for(int x = 0; x < cube_coordinates.size(); x++){
        cout << "(" << cube_coordinates[x].x << ", " << cube_coordinates[x].y << ", " << cube_coordinates[x].z << ") -> "
            << "(" << output[x].x << ", " << output[x].y << ", " << output[x].z << ")" << endl;
    }
    
    return 0;
}
int main(){
    cout << "Translation test..." << endl;
    translation_test();
    cout << "Scaling test..." << endl;
    scaling_test();
    cout << "Rotation test..." << endl;
    rotation_test();

    return 0;
}