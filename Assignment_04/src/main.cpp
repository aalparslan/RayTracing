#include "helper.hpp"
//#include "solution.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <jpeglib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
  // Notice for this HW there is no need for up vector
  // All references for the below variables are given w.r.t. (0, 0, 1)
  glm::vec3 position;
  // Angles in the assignment
  // Store in radians!!!
  float pitchAngle;
  float yawAngle;
  // Speed of the camera with no direction
  float speed;
};
Camera camera;

// Properties
int textureWidth, textureHeight;
// Hold if currently full screen or not
bool fullScreenMode = false;
// Hold the previous size of the window!
int windowModeXStart, windowModeYStart, windowModeXSize, windowModeYSize;

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




void calculateCamera(const Camera &camera){
  /*
   * After called, GL_MODELVIEW will be the loaded!
   * WARNING: THIS FUNCTION RESETS THE MODELVIEW MATRIX!
  */
  // Calculate the camera parameters using the camera properties
  glm::vec3 gazeVector = glm::normalize(glm::vec3(
    // x
    cos(camera.pitchAngle)*sin(camera.yawAngle),
    // y
    sin(camera.pitchAngle),
    // z
    cos(camera.pitchAngle)*cos(camera.yawAngle)
  ));

  // Calculate the camera position in time
  // TODO...

  // set up mvp...
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(
    // Eye position
    camera.position.x,
    camera.position.y,
    camera.position.z,
    // Already calculated vector
    camera.position.x + gazeVector.x,
    camera.position.y + gazeVector.y,
    camera.position.z + gazeVector.z,
    // Up vector (constant for this assignment)
    0.,
    1.,
    0.
  );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Below are given statically in the assignment text!
  gluPerspective(
    45.,
    1.,
    0.1,
    1000
  );
  glMatrixMode(GL_MODELVIEW);
}
void initCamera(int textureWidth, int textureHeight, Camera &camera){
  // The camera will be positioned initially at (w/2, w/10, -w/4) where w is the width of the texture image
  camera.position = glm::vec3(textureWidth / 2.0, textureWidth / 10.0, - textureWidth / 4.0);
  // Initial gaze
  camera.pitchAngle = 0;
  camera.yawAngle   = 0;
  // Initial speed
  camera.speed = 0;

  calculateCamera(camera);
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
    calculateCamera(camera);
  }else if(key == GLFW_KEY_D){
    // Camera look down
    camera.yawAngle = camera.yawAngle - 0.05;
    calculateCamera(camera);
  }else if(key == GLFW_KEY_S){
    camera.pitchAngle = camera.pitchAngle - 0.05;
    calculateCamera(camera);
  }else if(key == GLFW_KEY_W){
    camera.pitchAngle = camera.pitchAngle + 0.05;
    calculateCamera(camera);
  }else if(key == GLFW_KEY_I){
    initCamera(textureWidth, textureHeight, camera);
  }else if(key == GLFW_KEY_P && action == GLFW_PRESS){
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



void initialize(Camera &camera, int textureWidth, int textureHeight, vector<Index> &indices, vector<Vertex> &vertices){

    // 1) Initialize the vertices/indices
    initVerticesAndIndices(textureWidth, textureHeight, indices, vertices);


    // 2) Initialize the camera
    initCamera(textureWidth, textureHeight, camera);


    // 3) Initialize the shaders
    // Initizalize shaders with idProgramShader
    // TODO - Bunu da init etmek gerekiyor
    // string vertexShader = "src/shaders/shader.vert";
    // string fragmentShader = "src/shaders/shader.frag";
    // initShaders(idProgramShader, vertexShader , fragmentShader );
    // glUseProgram(idProgramShader);

    // 4) Initialize the GPU memory
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

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

}

void initializeOPENGL(int widthWindow, int heightWindow, int argc, char *argv[]){
  // TODO -> kanka burasi hata veriyorsa, __MACOS__ yerine baska bir sey olmasi gerekebilir
  #ifndef __MACOS__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);


  /// TRADITIONAL  MacOS FLAGS TRY TO REMOVE THEM LATER ON...
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  /////////////////////////////////////////////////////////
  #endif

  // Default ones...
  if (argc != 3) {
      printf("Please provide height and texture image files!\n");
      exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
      exit(-1);
  }

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

  // Keyboard interrupts
  glfwSetKeyCallback(win, keyCallback);

}
void denemeUcgenler(){
  // BUNLAR GECICI TEST ICIN
  glBegin(GL_TRIANGLES);
  glColor3f(0.9, 0.0, 0.0);
  glVertex3f(500.,100., 0.);
  glVertex3f(600.,100., 0.);
  glVertex3f(500.,200.,0.);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(0., 0.7, 0.);
  glVertex3f(500., 0., 100.);
  glVertex3f(600., 0., 100.);
  glVertex3f(500., 0., 600.);
  glEnd();
}
int main(int argc, char *argv[]) {
    vector<Index> indices;
    vector<Vertex> vertices;



    float heightFactor;

    // TODO -> bu statik mi kalacak ya?
    int widthWindow  = 1000;
    int heightWindow = 500;

    // Initialize the opengl framework
    initializeOPENGL(widthWindow, heightWindow, argc, argv);

    // From helper...
    initTexture(argv[1], argv[2], &textureWidth, &textureHeight);

    // Initialize all the remaining properties
    initialize(camera, textureWidth, textureHeight, indices, vertices);


    while(!glfwWindowShouldClose(win)) {
      // Kanka burda sanirim glDrawElements gibi bir sey olacak ama o kismi cozemedim, bakabilir misin?
      // glDrawElements(GL_TRIANGLES, numTriangles[1]*3, GL_UNSIGNED_INT,(GLvoid *)(sizeof(GLuint)*numTriangles[0]*3));

      //// Clear the view!
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //// Sizing the view
      int height;
      int width;
      // Get current size
      glfwGetFramebufferSize(win, &width, &height);
      // Set the rendering size
      glViewport(0, 0, width, height);

      //// Sadece denemek icin
      denemeUcgenler();


      glfwSwapBuffers(win);
      glfwPollEvents();
    }


    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
