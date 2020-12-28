#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MathematicalOperations.hpp"

using namespace std;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;


static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initialize(int& totalNumberOfFaces, GLuint& vboVertex, GLuint& vboNormal, GLuint& vboIndex){
    
    for(int i = 0; i < scene.meshes.size(); i++){
        for(int j = 0; j < scene.meshes[i].faces.size(); j++){
            totalNumberOfFaces++;
        }
    }
    
    vector<parser::Vec3f> normals(scene.vertex_data.size());
    GLint* indexArray = new GLint[totalNumberOfFaces * 3];
    GLfloat* normalArray = new GLfloat[scene.vertex_data.size() * 3];
    GLfloat* vertexArray = new GLfloat[scene.vertex_data.size() * 3];


    //Below code finds the normal of a vertex by adding all the adjacent faces' normals together.
    int indexOffset = 0;
    for(int i = 0; i < scene.meshes.size(); i++){
        for(int j = 0; j < scene.meshes[i].faces.size(); j++){
            parser::Face face = scene.meshes[i].faces[j];

            normals[face.v0_id -1] = MatOp::vectorAddition(normals[face.v0_id -1], face.normal);
            normals[face.v1_id -1] = MatOp::vectorAddition(normals[face.v1_id -1], face.normal);
            normals[face.v2_id -1] = MatOp::vectorAddition(normals[face.v2_id -1], face.normal);


            // store x,y,z values of the indices together
            indexArray[indexOffset++] = face.v0_id -1;
            indexArray[indexOffset++] = face.v1_id -1;
            indexArray[indexOffset++] = face.v2_id -1;

        }
    }

    // normalize normal vectors of every vertex and store x,y,z values of the normals together
    for(int i = 0; i < normals.size(); i++){
         normals[i] = MatOp::vectorNormalize( normals[i]);
        normalArray[i*3] = normals[i].x;
        normalArray[i*3 + 1] = normals[i].y;
        normalArray[i*3 + 2] = normals[i].z;
    }

    //store x,y,z values of the vertices together
    for(int i = 0; i < scene.vertex_data.size(); i++){
        vertexArray[i*3] = scene.vertex_data[i].x;
        vertexArray[i*3 + 1] = scene.vertex_data[i].y;
        vertexArray[i*3 + 2] = scene.vertex_data[i].z;
    }

    // give data to gpu
    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, scene.vertex_data.size() * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

    glGenBuffers(1, &vboNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
    glBufferData(GL_ARRAY_BUFFER, scene.vertex_data.size() * 3 * sizeof(GLfloat), normalArray, GL_STATIC_DRAW);
    
    glGenBuffers(1, &vboIndex);
    
    // Bind VBO for drawing array data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalNumberOfFaces * 3 * sizeof(GLuint), indexArray, GL_STATIC_DRAW);
    
    
    // Unbind any buffer object previously bound
    // Bind with 0 to switch back to default pointer operation
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Activate array-based data
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glShadeModel(GL_SMOOTH);
    
}

void setUpCamera(){

    // physical location of the camera - which direction it is pointing - what  the orientation is set here.
    parser::Vec3f imagePlaneCenter = MatOp::vectorAddition( scene.camera.position,
                                                           MatOp::vectorMultiplication(scene.camera.gaze, scene.camera.near_distance) );
    // projection properties of the camera like depth of view, fild of view  in x and y directions set here.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    parser::Vec4f nearPlane = scene.camera.near_plane;
    glFrustum(nearPlane.x, nearPlane.y, nearPlane.z, nearPlane.w, scene.camera.near_distance, scene.camera.far_distance);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(scene.camera.position.x, scene.camera.position.y, scene.camera.position.z,
              imagePlaneCenter.x, imagePlaneCenter.y, imagePlaneCenter.z,
              scene.camera.up.x,scene.camera.up.y ,scene.camera.up.z);

}



void turnOnLights(){
    glEnable(GL_LIGHTING);
    for(int i = 0; i < scene.point_lights.size(); i++){
        parser::PointLight light = scene.point_lights[i];
        glEnable(GL_LIGHT0 + i);
        GLfloat col[] = {light.intensity.x, light.intensity.y, light.intensity.z, 1.0f};
        GLfloat pos[] = {light.position.x, light.position.y, light.position.z, 1.0f};
        GLfloat ambientLight[4] = {scene.ambient_light.x, scene.ambient_light.y, scene.ambient_light.z, 1.0f};

        glLightfv(GL_LIGHT0 + i , GL_POSITION, pos);
        glLightfv(GL_LIGHT0 + i , GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0 + i , GL_DIFFUSE, col);
        glLightfv(GL_LIGHT0 + i , GL_SPECULAR, col);
    }
}

void turnOffLights(){
    glDisable(GL_LIGHTING);
    for(int i = 0; i < scene.point_lights.size(); i++){
        glDisable(GL_LIGHT0 + i);
    }

}



int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);
    int totalNumberOfFaces = 0;
    GLuint vboVertex;
    GLuint vboNormal;
    GLuint vboIndex;
    
    glfwSetErrorCallback(errorCallback);
    
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(win, keyCallback);
    
    initialize(totalNumberOfFaces, vboVertex, vboNormal, vboIndex);
    setUpCamera();
    turnOnLights();
    
    while(!glfwWindowShouldClose(win)) {
        
        //~~~Couple of TODOs~~~//
        // TODO: show FPS on the title
        // TODO: implement culling
        // TODO: test on ineks
        
        // draw objects all the time
        glClearColor(scene.background_color.x, scene.background_color.y,
                     scene.background_color.z, 1);
        glClearDepth(1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        
        // implement
        // Bind VBO for drawing array data
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);


        int Offset = 0;
        for(int i = 0; i < scene.meshes.size(); i++){
            parser::Mesh mesh = scene.meshes[i];
            // push and pop matrix methods used to transform a specific object in the scene.
            // in this case  they are called for every mesh object.
            glPushMatrix();
            for(int j =  mesh.transformations.size()-1; j >= 0; j--){


                // do transformations
                //not sure about order of applying transformations check this later...
                parser::Transformation transformation = mesh.transformations[j];
                
                if(transformation.transformation_type == "Translation"){
                    parser::Vec3f translation = scene.translations[transformation.id];
                    glTranslatef(translation.x,translation.y,translation.z);

                }else if(transformation.transformation_type == "Scaling"){
                    parser::Vec3f scaling = scene.scalings[transformation.id];
                    glScalef(scaling.x,scaling.y,scaling.z);

                }else if(transformation.transformation_type == "Rotation"){
                    parser:: Vec4f rotation = scene.rotations[transformation.id];
                    glRotatef(rotation.x, rotation.y, rotation.z, rotation.w);
                }else{
                    throw runtime_error("Error in Type Checking");
                }
            }
            // below code given in assingment pdf file for calculation of color.
            parser::Material material = scene.materials[mesh.material_id -1];

            GLfloat ambColor[4] = {material.ambient.x,material.ambient.y,material.ambient.z, 1.0};
            GLfloat diffColor[4] = {material.diffuse.x,material.diffuse.y,material.diffuse.z, 1.0};
            GLfloat specColor[4] = {material.specular.x,material.specular.y,material.specular.z, 1.0};
            GLfloat specExp [1] = {material.phong_exponent};
            glMaterialfv ( GL_FRONT , GL_AMBIENT , ambColor );
            glMaterialfv ( GL_FRONT , GL_DIFFUSE , diffColor );
            glMaterialfv ( GL_FRONT , GL_SPECULAR , specColor );
            glMaterialfv ( GL_FRONT , GL_SHININESS , specExp );
            
            
            mesh.mesh_type == "Wireframe" ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
           


            glDrawElements(GL_TRIANGLES, mesh.faces.size()*3, GL_UNSIGNED_INT, (void*)(sizeof(GLuint)*3*Offset));
            Offset += mesh.faces.size();
            glPopMatrix();
            

        }
        glfwSwapBuffers(win);
        glfwWaitEvents();
        
    }
    
    glfwDestroyWindow(win);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
    
    return 0;
}
