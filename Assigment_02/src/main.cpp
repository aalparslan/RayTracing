// Dosya uzun oldugu icin kafam karisti, bu dosyalara boldum simdilik
#include <iostream>
#include "parser.hpp"
#include "ppm.hpp"
#include <math.h>

#include "MathematicalOperations.hpp"
#include <thread>
#include "IntersectionCalculator.hpp"
#include "Transformator.hpp"
#include "ColorCalculator.hpp"
#include "SceneTransformations.hpp"

#define INFINITE 40000
using namespace std;

// Measure time
#include <chrono>
using namespace std::chrono;


const int numberOfCores = thread::hardware_concurrency();
//const int numberOfCores = 1;



void loadCamera(parser::Camera &x, parser::Camera &cam, parser::Vec3f &camUVector, float &pixelW, float &pixelH){
    cam.position = x.position;
    cam.gaze = x.gaze;
    cam.up = x.up;
    cam.near_plane = x.near_plane;
    cam.near_distance = x.near_distance;
    cam.image_width = x.image_width;
    cam.image_height = x.image_height;
    cam.image_name = x.image_name;
    
    pixelW = (cam.near_plane.y - cam.near_plane.x)/ (float) cam.image_width;
    
    pixelH = (cam.near_plane.w - cam.near_plane.z) / (float) cam.image_height;
    
    camUVector = MatOp::vectorCrossProduct(cam.gaze, cam.up);
    
    
    camUVector = MatOp::vectorNormalize(camUVector);
    
    cam.up = MatOp::vectorCrossProduct(camUVector, cam.gaze);
    cam.up = MatOp::vectorNormalize(cam.up);
    
    cam.gaze = MatOp::vectorNormalize(cam.gaze);
}



void initImage(parser::Scene *scene, unsigned char* &immage, parser::Camera cam){
    int max = cam.image_width * cam.image_height * 3;
    immage =  new unsigned char [max];
    
    // Below sets the background color
    int  k =0;
    for(int i =0 ; i < cam.image_width; i++){
        for(int j =0; j < cam.image_height; j++){
            
            
            immage[k++] = (unsigned char) scene->background_color.x;
            immage[k++] = (unsigned char) scene->background_color.y;
            immage[k++] = (unsigned char) scene->background_color.z;
            
            
        }
    }
    
    
}
parser::Ray generateRay(int i, int j, float pixelW, float pixelH, parser::Camera cam, parser::Vec3f camUVector){
    
    parser::Vec3f m;
    parser::Vec3f q;
    parser::Vec3f result;
    float su, sv;
    
    m = MatOp::vectorAddition(cam.position, MatOp::vectorMultiplication(cam.gaze, cam.near_distance));
    q = MatOp::vectorAddition(m, MatOp::vectorAddition(MatOp::vectorMultiplication(camUVector,cam.near_plane.x), MatOp::vectorMultiplication(cam.up,cam.near_plane.w)));
    
    su = 0.5* pixelW + j*pixelW;
    sv = 0.5* pixelH + i*pixelH;
    
    result = MatOp::vectorAddition(q, MatOp::vectorAddition(MatOp::vectorMultiplication(camUVector, su), MatOp::vectorMultiplication(cam.up, -sv)));
    
    
    result.x = result.x - cam.position.x;
    result.y = result.y - cam.position.y;
    result.z = result.z - cam.position.z;
    
    result = MatOp::vectorNormalize(result);
    
    parser::Ray ray;
    ray.a = cam.position;
    ray.b = result;
    return ray;
    
}
void processImage(parser::Camera cam, parser::Scene *scene, int image_width, int finishHeight, int startingHeight, IntersectionCalculator *ic_pointer, ColorCalculator *cc, float pixelW, float pixelH, parser::Vec3f camUVector, unsigned char* immage, vector<parser::TextureObject> textureObjects){
    // Mind the reference!
    IntersectionCalculator &ic = *ic_pointer;
    
    
    int fark = finishHeight - startingHeight;
    
    for (int i = startingHeight; i < finishHeight; i++){
        cout<<100*(i - startingHeight)/ fark<<"%"<<endl;
        for(int j = 0; j < cam.image_width; j++){
            
            parser::Ray ray;
            
            ray = generateRay(i, j, pixelW, pixelH, cam, camUVector);
            
            IntersectionCalculator::IntersectionData intersection; // The attributes are the same for all three
            intersection = ic.intersectRay(ray, 1); //treshold 1 cunku tresholdun 1 oldugu nokta image plane 1 den kucuk oldugu noktalardaki kesisimler image planenin arkasinda kalacagindan gorunmeyecekler.

            if(intersection.t < INFINITE){
                parser::Vec3f color = cc->computeColor(ray, intersection, scene->max_recursion_depth,
                                                       scene, ic, textureObjects); // just one function for coloring

                
                int wherePixelStarts = i*(cam.image_width)*3 + j*3;


                immage[wherePixelStarts]    = (unsigned char) color.x;
                immage[wherePixelStarts + 1] = (unsigned char) color.y;
                immage[wherePixelStarts + 2] = (unsigned char) color.z;

            }
        }
    }
    
}
void createTextureObj(vector<parser::TextureObject> &textureObjects, parser::Scene *scene){
    
    
    for(int i = 0; i < scene->textures.size(); i++){
        parser::Texture texture = scene->textures[i];
        parser::TextureObject obj  = *new parser::TextureObject(texture);
        textureObjects.push_back(obj);
    }
    
}




int main(int argc, char* argv[])
{
    // Measure time
    auto start = high_resolution_clock::now();
    
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    
    scene.loadFromXml(argv[1]);
    
    
    vector<parser::TextureObject> textureObjects;
    createTextureObj( textureObjects, &scene);
    
    
    
    
    
    /*
     * Model transformations here
     * Notice model transformations are applied only
     * once; hence, all cameras will see the same
     * transformed system.
     */
    // Save face ids for texture mapping
    
    
    // Apply to triangles
    SceneTransformations::applyTriangleModelTransformations(scene);
    // Appy to meshes
    SceneTransformations::applyMeshModelTransformations(scene);
    
    // TODO - SPHERES! -> texture ile ic ice olcak bu!
    SceneTransformations::applySphereModelTransformations(scene);
    
    
    // Create variables AFTER object transformation is done for the scene
    IntersectionCalculator ic(scene);
    ColorCalculator cc(&scene);
    
    for(auto x = scene.cameras.begin(); x < scene.cameras.end(); x++){
        
        
        // Set up camera
        parser::Camera cam;
        parser::Vec3f camUVector;
        float pixelW, pixelH;
        loadCamera(*x, cam, camUVector, pixelW, pixelH);
        
        // Create image
        unsigned char* immage;
        initImage(&scene, immage, cam);
        
        
        
        const int width = (*x).image_width;
        const int height = (*x).image_height;
        if(numberOfCores == 0 || height < numberOfCores){
            processImage(cam,  &scene, width , height, 0, &ic, &cc, pixelW, pixelH, camUVector, immage, textureObjects);
        }else{
            thread* threads = new thread[numberOfCores];
            const int heightIncrease = height/numberOfCores;
            
            for(int i = 0; i < numberOfCores; i++){
                const int min_height = i*heightIncrease;
                
                if(i+1 != numberOfCores){
                    const int max_height = (i+1)*heightIncrease;
                    threads[i] = thread(processImage, cam, &scene, width, max_height, min_height, &ic, &cc, pixelW,
                                        pixelH, camUVector, immage, textureObjects);
                }else{
                    const int max_height = height;
                    threads[i] = thread(processImage, cam, &scene, width, max_height, min_height, &ic, &cc, pixelW,
                                        pixelH, camUVector, immage, textureObjects);
                }
            }
            
            for(int i =0; i < numberOfCores; i++){
                threads[i].join();
            }
            delete[] threads;
        }
        
        
        // For every camera write another ppm file image.
        write_ppm(cam.image_name.c_str(), immage, cam.image_width, cam.image_height);
        
        // Free memory
        delete[] immage;
    }
    
    
    
    // Print time passed
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    
    std::cout << "Time passed : " << duration.count() / 1000 / 1000 << " seconds" << std::endl;
    
    return 0;
    
}
