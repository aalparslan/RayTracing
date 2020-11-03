#include "helper.hpp"

double intersectTriangle(parser::Ray ray, parser::Triangle triangle){
    /*
     (Snyder & Barr, 1987) method
     Textbook Page 78 notation used
     */
    // TODO - ray.a balangic noktasi diye varsayiyorum, kontrol etcem
    // TODO - untested!
    // Ray constants...
    double x_e = ray.a.x;
    double y_e = ray.a.y;
    double z_e = ray.a.z;

    double x_d = ray.b.x;
    double y_d = ray.b.y;
    double z_d = ray.b.z;

    // Triangle constants...
    double x_a = vertexData_PTR[triangle.indices.v0_id].x;
    double y_a = vertexData_PTR[triangle.indices.v0_id].y;
    double z_a = vertexData_PTR[triangle.indices.v0_id].z;

    double x_b = vertexData_PTR[triangle.indices.v1_id].x;
    double y_b = vertexData_PTR[triangle.indices.v1_id].y;
    double z_b = vertexData_PTR[triangle.indices.v1_id].z;

    double x_c = vertexData_PTR[triangle.indices.v2_id].x;
    double y_c = vertexData_PTR[triangle.indices.v2_id].y;
    double z_c = vertexData_PTR[triangle.indices.v2_id].z;


    // Matrix elements...  
    double a = x_a - x_b;
    double b = y_a - y_b;
    double c = z_a - z_b;
    
    double d = x_a - x_c;
    double e = y_a - y_c;
    double f = z_a - z_c;

    double g = x_d;
    double h = y_d;
    double i = z_d;

    // Solution variables...
    double j = x_a - x_e;
    double k = y_a - y_e;
    double l = z_a - z_e;


    // Composite variables...
    double M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);


    // Main variables
    double beta  = j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g);
    double gamma = i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c);
    double t     = f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c);

    // Normalization...
    beta  = beta  / M;
    gamma = gamma / M;
    t     = t     / M * (-1);

    // TODO -> t_0 icin e noktasini, yani camera position aliyorum, bu yanlis olabilir mi?? cunku canvas ile kamera arasindaysa gormemeli sanirim? ya da gormeli mi :)
    if (
        t > 0     || t < 100   ||
        gamma < 0 || gamma > 1 ||
        beta < 0  || beta > 1
    ){
        // TODO -> t < 100 hesaplamasi yapilcak
        return -1;
    }else{
        return t;
    }


}
