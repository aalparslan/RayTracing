#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "helper.h"

using namespace std;

static GLFWwindow* win = NULL;
int widthWindow  = 1000;
int heightWindow = 1000;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

// Buffers
GLuint idVertexBuffer;
GLuint idIndexBuffer;

struct Vertex {
    //vertex pos
    glm::vec3 vPosition;
    // texture pos
    glm::vec2 textureCoordinate;
    //normal
    glm::vec3 normal;
};

int widthTexture, heightTexture;

//Camera initialCamera;
struct Camera {
    // Notice for this HW there is no need for up vector
    // All references for the below variables are given w.r.t. (0, 0, 1)
    glm::vec3 gaze;
    glm::vec3 up;
    glm::vec3 position;
    // Angles in the assignment
    // Store in radians!!!
    float pitchAngle;
    float yawAngle;
    // Speed of the camera with no direction
    float speed;
};
Camera camera;

// matrices
glm::mat4 MVP;
glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;


glm::vec3 lightPosition;

// locations
int locTextureHeight, locTextureWidth, locTexture, locMVP, locHeightFactor, locCameraPosition, locLightPosition;

// Hold if currently full screen or not
bool fullScreenMode = false;
bool initGeo = false;
// Hold the previous size of the window!
int windowModeXStart, windowModeYStart, windowModeXSize, windowModeYSize;

void calculateCamera(vector<int> &indices, float &heightFactor, float &aspectRatio, float &nearPlane, float &farPlane,  float &YfieldOfView){
    /*
     * After called, GL_MODELVIEW will be the loaded!
     * WARNING: THIS FUNCTION RESETS THE MODELVIEW MATRIX!
     * This calculates the next position of the camera.
     */
    // Calculate the camera parameters using the camera properties
    glClearColor(0,0,0,1);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.gaze = glm::normalize(glm::vec3(
                                                    // x
                                                    cos(glm::radians(camera.pitchAngle))*cos(glm::radians(camera.yawAngle)),
                                                    // y
                                                    sin(glm::radians(camera.pitchAngle - 45.0) ), ///check
                                                    // z
                                                    cos(glm::radians(camera.pitchAngle))*sin(glm::radians(camera.yawAngle))
                                                    ));
    
    // Calculate the camera position in time
    camera.position = camera.position + camera.speed * camera.gaze;
    
    View = glm::lookAt(camera.position, camera.position + camera.gaze, camera.up);
    Projection = glm::perspective(YfieldOfView, aspectRatio, nearPlane, farPlane);
    MVP = Projection * View * Model;

    // Send new geometry to OpenGL
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(MVP));
    glUniform3fv(locCameraPosition, 1, glm::value_ptr(camera.position));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    
}



void initGeometry(float &heightFactor, float &aspectRatio, float &nearPlane, float &farPlane,  float &YfieldOfView){
    // The camera will be positioned initially at (w/2, w/10, -w/4) where w is the width of the texture image
    camera.position = glm::vec3(widthTexture / 2.0, widthTexture / 10.0, - widthTexture / 4.0);
    camera.gaze = glm::vec3(0.0, 0.0, 0.1);
    camera.up = glm::vec3(0.0, 1.0, 0.0);

    //init mvp
    Model = glm::mat4(1.0);
    View = glm::lookAt(camera.position, camera.position + camera.gaze, camera.up);
    Projection = glm::perspective(YfieldOfView, aspectRatio, nearPlane, farPlane);
    MVP = Projection * View * Model;

    // initialize light as specified in the pdf.
    lightPosition = glm::vec3(widthTexture/ 2.0, 100, heightTexture / 2.0);
    // Initial gaze
    camera.pitchAngle = 45.0;
    camera.yawAngle   = 90.0;

    // Initial speed
    camera.speed = 0;
    heightFactor = 10;
}

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    /*
     * Alttaki yerde sirayla bakiyoruz...
     * OPENGL sebebiyle kamerayi local yapamiyoruz, global olmak zorunda :/
     */

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }else if(key == GLFW_KEY_A){
        // Camera look up
        camera.yawAngle = camera.yawAngle + 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_D){
        // Camera look down
        camera.yawAngle = camera.yawAngle - 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_S){
        camera.pitchAngle = camera.pitchAngle - 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_W){
        camera.pitchAngle = camera.pitchAngle + 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_I){
        initGeo = true;
    }else if(key == GLFW_KEY_Y){
        camera.speed += 0.01;
    }else if(key == GLFW_KEY_H){
        camera.speed -= 0.01;
    }else if(key == GLFW_KEY_P){
        if(!fullScreenMode){
            // Do not forget to store the current view!
            glfwGetWindowPos(window, &windowModeXStart, &windowModeYStart);
            glfwGetWindowSize(window, &windowModeXSize, &windowModeYSize);

            fullScreenMode = true;

            // Get the video mode
            const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            // Set the window to the expected mode!
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, videoMode->width, videoMode->height, 0);
        }else{
            // Set windowed operation flag
            fullScreenMode = false;

            // Set back!
            glfwSetWindowMonitor(window, NULL, windowModeXStart, windowModeYStart, windowModeXSize, windowModeYSize, 0);
        }
    }
}

void initVerticesAndIndices(    vector<int> &indices,
                            vector<Vertex> &vertices){
    // initiate indices

    for(int i = 1; i < heightTexture+1; i++){
        for(int j = 1; j < widthTexture+1; j++){

            // asagidaki if en sagdaki vertexlerle en soldaki arasinda ucgen olusturmayip diger durumlarda olusturuyor.
            if(((i)*widthTexture + j + 1) % (widthTexture +1) != 0){
                
                indices.push_back((i-1)*widthTexture + j-1);
                indices.push_back((i)*widthTexture + j );
                indices.push_back((i-1)*widthTexture + j);
                
                indices.push_back((i-1)*widthTexture + j );
                indices.push_back((i)*widthTexture + j );
                indices.push_back((i)*widthTexture + j + 1);
            }
        }
    }

   //  initiate vertices
    for(int i = 0; i < heightTexture+1; i++){
        for(int j = 0; j < widthTexture+1; j++){
            Vertex v;
            v.vPosition = glm::vec3(j, 0.0, i);
            v.normal = glm::vec3(0.0); // vertex shader will calculate normal
            v.textureCoordinate = glm::vec2(1 - ( ((float) j) / widthTexture ), 1 - (((float) i) / heightTexture) );
            vertices.push_back(v);
        }
    }
}

// uniforms are initialized below for shaders' usage
void initializeUniforms( float &heightFactor){
    
    locMVP = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(MVP));
    locHeightFactor = glGetUniformLocation(idProgramShader,"heightFactor");
    glUniform1f(locHeightFactor, heightFactor);
    locTextureHeight = glGetUniformLocation(idProgramShader,"heightTexture");
    glUniform1i(locTextureHeight, heightTexture);
    locCameraPosition = glGetUniformLocation(idProgramShader,"cameraPos");
    glUniform3fv(locCameraPosition, 1, glm::value_ptr(camera.position));
    locTextureWidth = glGetUniformLocation(idProgramShader,"widthTexture");
    glUniform1i(locTextureWidth, widthTexture);
    locLightPosition = glGetUniformLocation(idProgramShader, "lightPosition");
    glUniform3fv(locLightPosition, 1, glm::value_ptr(lightPosition));
    locTexture = glGetUniformLocation(idProgramShader, "rgbTexture");
    glUniform1i(locTexture, 1);
}


void initializeBuffers(    vector<int> &indices,
                       vector<Vertex> &vertices){
    
    // 1) Initialize the vertices/indices
    initVerticesAndIndices( indices, vertices);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //Create vbo for vertices
    glGenBuffers(1, &idVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, idVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Create vbo for indices
    glGenBuffers(1, &idIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(Vertex,textureCoordinate));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
}

void initializeOPENGL(int argc, char *argv[]){
    
    // Default ones...
    if (argc != 3) {
        printf("Please provide height and texture image files!\n");
        exit(-1);
    }
    
    glfwSetErrorCallback(errorCallback);
    
    if (!glfwInit()) {
        exit(-1);
    }
    
#ifndef __MACOS__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glewExperimental = GL_TRUE;
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    
    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);
    
    if (!win) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(win);
    
    glViewport(0, 0, widthWindow, heightWindow);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        
        glfwTerminate();
        exit(-1);
    }
    
    // Keyboard interrupts
    glfwSetKeyCallback(win, keyCallback);
}

int main(int argc, char *argv[]) {

    // containers for data
    vector<int> indices;
    vector<Vertex> vertices;

    // Properties
    float heightFactor = 10.f;
    float aspectRatio = 1.0;
    float nearPlane = 0.1;
    float farPlane = 1000.;
    float YfieldOfView = 45.0;
    
    // Initialize the opengl framework
    initializeOPENGL(argc, argv);
    
    //  Initialize the shaders
    string vertexShader = "src/shaders/shader.vert";
    string fragmentShader = "src/shaders/shader.frag";
    initShaders(idProgramShader, vertexShader , fragmentShader );
    // From helper...
    initTexture(argv[1], argv[2], &widthTexture, &heightTexture);
    // Initialize all the remaining properties
    initializeBuffers( indices, vertices);
    initGeometry(  heightFactor,  aspectRatio,  nearPlane,  farPlane,   YfieldOfView);
    glUseProgram(idProgramShader);
    // initialize uniforms
    initializeUniforms( heightFactor);
    
    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(win)) {
        
        calculateCamera(indices,  heightFactor,  aspectRatio,  nearPlane,  farPlane,   YfieldOfView);
        if(initGeo){
             initGeometry(  heightFactor,  aspectRatio,  nearPlane,  farPlane,   YfieldOfView);
            initGeo = false;
        }
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
