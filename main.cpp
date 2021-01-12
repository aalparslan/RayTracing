#include "helper.h"
//#include "solution.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

using namespace std;
static GLFWwindow* win = NULL;
int widthWindow = 1000, heightWindow = 1000;

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

int textureWidth, textureHeight;
float heightFactor;



//Camera initialCamera;
struct Camera {
    glm::vec3 posiotion;
    glm::vec3 gaze;
    glm::vec3 up;
    glm::vec3 left;
    
};

Camera camera;
glm::vec3 lightPos;
GLuint depthMapFBO;
GLuint depthCubemap;
bool lightPosFlag = false;

// matrices
glm::mat4 MVP;
glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;

struct Vertex {
    //vertex pos
    glm::vec3 vPosition;
    // texture pos
    glm::vec2 tPosition;
    
};

struct Index {
    glm::vec3 pos;
};

vector<Index> indices;
vector<Vertex> vertices;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initVerticesAndIndices(){
    
    // initialize indices
    int k = 0;
    for(int i = 0; i < textureHeight; i++){
        for(int j = 0; j < textureWidth; j++){
            
            // triangle1 in pixel
            // left upperside, left lowerside,  right upperside
            glm::vec3 indice1 = glm::vec3(i*textureWidth + j,(i+1)*textureWidth + j,i*textureWidth + j + 1);
            //triangle2
            //left lowerside, right lowerside, right upperside
            glm::vec3 indice2 = glm::vec3((i+1)*textureWidth + j,(i+1)*textureWidth + j+1,i*textureWidth + j + 1);
        
            indices[k].pos = indice1;
            k++;
            indices[k].pos = indice2;
            k++;
            
            
            // initialize vertices
            // x,y,z
            glm::vec3 vPosition = glm::vec3(j,0.0,i);
            // u,v
            glm::vec2 tPosition = glm::vec2(1 - ((float) j)/textureWidth , 1 - ((float) i)/textureHeight);
            
            vertices[i*textureWidth + j].vPosition = vPosition;
            vertices[i*textureWidth + j].tPosition = tPosition;
        }
    }
}

void initCamera(){
    //The camera will be positioned initially at (w/2, w/10, -w/4) where w is the width of the texture image
    camera.posiotion = glm::vec3(textureWidth/2.0, textureWidth/ 10.0, -textureWidth/ 4.0);
    camera.gaze = glm::vec3(0.0, 0.0, 1.0);
    camera.up = glm::vec3(0.0, 1.0, 0.0);
    camera.left = glm::vec3(glm::normalize(glm::cross(camera.gaze, camera.up)));
    
    // set up mvp...
    
    
}

void initialize(){
    
    // initizalize shaders with idProgramShader
    string vertexShader = "shader.vert";
    string fragmentShader = "shader.frag";
    initShaders(idProgramShader, vertexShader , fragmentShader );
    
    
    //create a VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    initVerticesAndIndices();
    
    //Create vbo for vertices
    glGenBuffers(1, &idVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, idVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
    // Create vbo for indices
    glGenBuffers(1, &idIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index)*indices.size(), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 1, GL_V3F, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2),(void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_V2F, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void *) (3 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    glUseProgram(idProgramShader);
    initCamera();
    
    
    
    
    
    
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("Please provide height and texture image files!\n");
        exit(-1);
    }
    
    glfwSetErrorCallback(errorCallback);
    
    if (!glfwInit()) {
        exit(-1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
     //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local
    
    
    /// TRADITIONAL  MacOS FLAGS TRY TO REMOVE THEM LATER ON...
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /////////////////////////////////////////////////////////
    
    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);
    
    if (!win) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(win);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        
        glfwTerminate();
        exit(-1);
    }
    
    
    initialize();
    
 
    
    glfwSetKeyCallback(win, keyCallback);
    
    initTexture(argv[1], argv[2], &textureWidth, &textureHeight);
    
    
    while(!glfwWindowShouldClose(win)) {
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    
    
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
