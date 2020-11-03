#include "helper.hpp"

std::pair<double, parser::Vec3f>  intersectTriangle(const parser::Ray &ray, const parser::Triangle& triangle, std::vector<parser::Vec3f> &vertexData){
    /*
     Return t as double and normal vector as Vec3f.
     t = -1 if no intersection exists.

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
    // Notice the indices start from 1, so decrement is needed
    double x_a = vertexData[triangle.indices.v0_id-1].x;
    double y_a = vertexData[triangle.indices.v0_id-1].y;
    double z_a = vertexData[triangle.indices.v0_id-1].z;

    double x_b = vertexData[triangle.indices.v1_id-1].x;
    double y_b = vertexData[triangle.indices.v1_id-1].y;
    double z_b = vertexData[triangle.indices.v1_id-1].z;

    double x_c = vertexData[triangle.indices.v2_id-1].x;
    double y_c = vertexData[triangle.indices.v2_id-1].y;
    double z_c = vertexData[triangle.indices.v2_id-1].z;


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

    // Calculate the edge vectors
    // Direction is from second vertex to first and third
    parser::Vec3f edge1, edge2, normal;

    edge1.x = x_a - x_b;
    edge1.y = y_a - y_b;
    edge1.z = z_a - z_b;

    edge2.x = x_c - x_b;
    edge2.y = y_c - y_b;
    edge2.z = z_c - z_b;
    
    // TODO - edge order
    normal = vectorNormalize(vectorCrossProduct(edge2, edge1));

    // TODO -> t_0 icin e noktasini, yani camera position aliyorum, bu yanlis olabilir mi?? cunku canvas ile kamera arasindaysa gormemeli sanirim? ya da gormeli mi :)
    if (
        t < 0     ||
        gamma < 0 || gamma + beta > 1 ||
        beta < 0
    ){
        return std::pair<double, parser::Vec3f>(-1, normal);
    }else{
        return std::pair<double, parser::Vec3f>(t, normal);
    }


}
