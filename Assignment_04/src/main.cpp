#include "helper.hpp"
//#include "solution.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

using namespace std;
static GLFWwindow* win = NULL;

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



//Camera initialCamera;
struct Camera {
    glm::vec3 position;
    glm::vec3 gaze;
    glm::vec3 up;
    glm::vec3 left;

};


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



static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initVerticesAndIndices(int textureWidth, int textureHeight, vector<Index> &indices, vector<Vertex> &vertices){
    indices  = vector<Index>(2*textureWidth*textureHeight);
    vertices = vector<Vertex>(2*textureWidth*textureHeight);

    // Calculate the data
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

void initCamera(int textureWidth, int textureHeight, Camera &camera){
    // The camera will be positioned initially at (w/2, w/10, -w/4) where w is the width of the texture image
    camera.position = glm::vec3(textureWidth / 2.0, textureWidth / 10.0, - textureWidth / 4.0);
    // Initial gaze
    camera.gaze     = glm::vec3(0.0, 0.0, 1.0);
    camera.up       = glm::vec3(0.0, 1.0, 0.0);

    // set up mvp...
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
      // Eye position
      camera.position.x,
      camera.position.y,
      camera.position.z,
      // Reference point position
      camera.gaze.x,
      camera.gaze.y,
      camera.gaze.z,
      // Up vector
      camera.up.x,
      camera.up.y,
      camera.up.z
    );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
      60.,
      (float) textureWidth / textureHeight,
      1,
      1000
    );
    glMatrixMode(GL_MODELVIEW);




}

void initialize(Camera &camera, int textureWidth, int textureHeight, vector<Index> &indices, vector<Vertex> &vertices){

    // initizalize shaders with idProgramShader
    // string vertexShader = "src/shaders/shader.vert";
    // string fragmentShader = "src/shaders/shader.frag";
    // initShaders(idProgramShader, vertexShader , fragmentShader );


    //create a VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    initVerticesAndIndices(textureWidth, textureHeight, indices, vertices);

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
    initCamera(textureWidth, textureHeight, camera);






}

void initializeOPENGL(){
  // TODO -> kanka burasi hata veriyorsa, __MACOS__ yerine baska bir sey olmasi gerekebilir
  #ifndef __MACOS__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

 // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local


  /// TRADITIONAL  MacOS FLAGS TRY TO REMOVE THEM LATER ON...
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  /////////////////////////////////////////////////////////
  #endif
}

int main(int argc, char *argv[]) {
    vector<Index> indices;
    vector<Vertex> vertices;

    Camera camera;
    int textureWidth, textureHeight;
    float heightFactor;




    if (argc != 3) {
        printf("Please provide height and texture image files!\n");
        exit(-1);
    }

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(-1);
    }

    // TODO -> bu statik mi kalacak ya?
    int widthWindow  = 1000;
    int heightWindow = 500;
    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(win);

    // From helper...
    initTexture(argv[1], argv[2], &textureWidth, &textureHeight);


    initialize(camera, textureWidth, textureHeight, indices, vertices);



    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

        glfwTerminate();
        exit(-1);
    }


    // set up mvp...
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
      // Eye position
      0,
      0,
      -10,
      // Reference point position
      0,
      0,
      0,
      // Up vector
      0,
      1,
      0
    );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
      60.,
      (float) widthWindow / heightWindow,
      1,
      1000
    );
    glMatrixMode(GL_MODELVIEW);


    glfwSetKeyCallback(win, keyCallback);




    while(!glfwWindowShouldClose(win)) {
      glBegin(GL_TRIANGLES);
      glColor3f(0.7, 0.7, 0.7);
      glVertex3f(  0.,  0.,  0.);
      glVertex3f(200.,  0.,  0.);
      glVertex3f(100.,100.,  0.);
      glEnd();

      glfwSwapBuffers(win);
      glfwPollEvents();
    }


    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
