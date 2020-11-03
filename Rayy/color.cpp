#include "helper.hpp"

Color calculatePixelColor(int i, int j, parser::Vec3f intersectionPoint, parser::Vec3f normal, parser::Material material, std::vector<parser::PointLight> &lightSources){
    /*
    Calculate superposition of three shading techniques. Ambient, Diffuse, Specular.
    Source is 83. page of the textbook.
    */
    parser::Vec3f pixelColor;
    
    // Ambient shading...
    // Calculating ambient light x material light
    pixelColor.x = scenePTR->ambient_light.x * material.ambient.x;
    pixelColor.y = scenePTR->ambient_light.y * material.ambient.y;
    pixelColor.z = scenePTR->ambient_light.z * material.ambient.z;
    
    for(auto lightSourcePTR = lightSources.begin(); lightSourcePTR < lightSources.end(); lightSourcePTR++){
        parser::PointLight lightSource = *lightSourcePTR;

        // Diffuse shading...
        parser::Vec3f w_i = vectorDifference(lightSource.position, intersectionPoint);
        double sourceDist = vectorLength(w_i);
        parser::Vec3f I_p = vectorDivision(lightSource.intensity, (sourceDist*sourceDist));
        
        double theta = std::max(0., dot(w_i, normal));
        pixelColor.x = pixelColor.x + material.diffuse.x * cos(theta) * I_p.x;
        pixelColor.y = pixelColor.y + material.diffuse.y * cos(theta) * I_p.y;
        pixelColor.z = pixelColor.z + material.diffuse.z * cos(theta) * I_p.z;

        // Specular shading
        parser::Vec3f w_o = vectorMultiplication(intersectionPoint, -1.0);

        parser::Vec3f h   = vectorDivision(
            vectorAddition(w_i, w_o),
            vectorLength(
                vectorAddition(w_i, w_o)
            )
        );

        double theta = std::max(0., dot(normal, h));

        pixelColor.x = pixelColor.x + material.specular.x * cos(theta) * I_p.x; // Irradiance is the same here
        pixelColor.y = pixelColor.y + material.specular.y * cos(theta) * I_p.y; // Irradiance is the same here
        pixelColor.z = pixelColor.z + material.specular.z * cos(theta) * I_p.z; // Irradiance is the same here
        
    }
    

    // TODO - ASSUMING X,Y,Z -> R,G,B ??? DOGRU MU??
    Color pixelColorD = {0, 0, 0};
    if (pixelColor.x >= 255){
        pixelColorD.R = 255;
    }
    if (pixelColor.y >= 255){
        pixelColorD.G = 255;
    }
    if (pixelColor.z >= 255){
        pixelColorD.B = 255;
    }

    if (pixelColor.x < 0 || pixelColor.y < 0 || pixelColor.z < 0)
        std::cout << "DEBUG! Unexpected variable!";

    return pixelColorD;
}