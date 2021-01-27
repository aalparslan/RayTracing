#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "helper.hpp"

using namespace std;

static GLFWwindow* win = NULL;
// TODO -> simdilik test icin degistir
// int widthWindow  = 1000;
// int heightWindow = 1000;
int widthWindow  = 500;
int heightWindow = 500;

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
// initial camera parameters -> kamerayi sifirlamak icin gerekiyor
Camera initialCamera;
// our camera
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
      cos(camera.pitchAngle)*sin(camera.yawAngle),
      // y
      sin(camera.pitchAngle),
      // z
      cos(camera.pitchAngle)*cos(camera.yawAngle)
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
    initialCamera.position = glm::vec3(widthTexture / 2.0, widthTexture / 10.0, - widthTexture / 4.0);
    initialCamera.gaze = glm::vec3(0.0, 0.0, 0.1);
    initialCamera.up = glm::vec3(0.0, 1.0, 0.0);

    camera = initialCamera;
    //init mvp
    Model = glm::mat4(1.0);
    View = glm::lookAt(camera.position, camera.position + camera.gaze, camera.up);
    Projection = glm::perspective(YfieldOfView, aspectRatio, nearPlane, farPlane);
    MVP = Projection * View * Model;

    // initialize light as specified in the pdf.
    lightPosition = glm::vec3(widthTexture/ 2.0, 100, heightTexture / 2.0);
    // Initial gaze - RADIANS!!!!!
    camera.pitchAngle = 0.;
    camera.yawAngle   = 0.;

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
    }else if(key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        // Camera look up
        camera.yawAngle = camera.yawAngle + 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        // Camera look down
        camera.yawAngle = camera.yawAngle - 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        camera.pitchAngle = camera.pitchAngle - 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        camera.pitchAngle = camera.pitchAngle + 0.05;
        // calculateCamera(camera);
    }else if(key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        camera = initialCamera;
    }else if(key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        camera.speed += 0.01;
    }else if(key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)){
        camera.speed -= 0.01;
    }else if(key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera.speed = 0.0;
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
    }else if(key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.y += 5;
      cout << lightPosition.y << endl;
    }else if(key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.y -= 5;
      cout << lightPosition.y << endl;
    }else if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.x -= 5;
    }else if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.x += 5;
    }else if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.z -= 5;
    }else if(key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      lightPosition.z += 5;
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



    //Create vbo for vertices
    glGenBuffers(1, &idVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, idVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Create vbo for indices
    glGenBuffers(1, &idIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);


//    GLint mPosition = glGetAttribLocation(idProgramShader, "position");
//    glVertexAttribPointer(mPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
//    glEnableVertexAttribArray(0);
//
//    GLint mTexCoord = glGetAttribLocation(idProgramShader,"textCoord");
//    glVertexAttribPointer(mTexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(Vertex,textureCoordinate));
//    glEnableVertexAttribArray(1);
//
//
//    GLint mNormal = glGetAttribLocation(idProgramShader,"normal");
//    glVertexAttribPointer(mNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(Vertex, normal));
//    glEnableVertexAttribArray(2);
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

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glewExperimental = GL_TRUE;
   // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(-1);
    }

    // Keyboard interrupts
    glfwSetKeyCallback(win, keyCallback);
    glfwMakeContextCurrent(win);
    printf("OpenGL version: %s - %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));



    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

        glfwTerminate();
        exit(-1);
    }
     glfwSwapInterval(1);



}

int main(int argc, char *argv[]) {

    ////////////////////
    GLuint vertex_buffer;
    GLuint normal_buffer;
    GLuint tcoord_buffer;
    GLuint index_buffer;

    vector<GLfloat> vdata;
    vector<GLfloat> tdata;
    vector<GLfloat> ndata;
    vector<GLuint> idata;
    ///////////////////////

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
    //////////////////////////
    initTexture(argv[1], argv[2], &widthTexture, &heightTexture);
    initVerticesAndIndices( indices, vertices);

    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &normal_buffer);
    glGenBuffers(1, &tcoord_buffer);
    glGenBuffers(1, &index_buffer);

    int totalnumberOfVertices = vertices.size();

    for(vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it){
        ndata.push_back(it->normal.x);
        ndata.push_back(it->normal.y);
        ndata.push_back(it->normal.z);

        vdata.push_back(it->vPosition.x);
        vdata.push_back(it->vPosition.y);
        vdata.push_back(it->vPosition.z);

        tdata.push_back(it->textureCoordinate.x);
        tdata.push_back(it->textureCoordinate.y);
    }
    for(vector<int>::iterator it = indices.begin(); it != indices.end(); ++it){
        idata.push_back(*it);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*totalnumberOfVertices, vdata.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*totalnumberOfVertices, ndata.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*totalnumberOfVertices, tdata.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), idata.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    /////////////////

    //  Initialize the shaders
    string vertexShader = "src/shaders/shader.vert";
    string fragmentShader = "src/shaders/shader.frag";
    initShaders(idProgramShader, vertexShader , fragmentShader );
    glUseProgram(idProgramShader);
    /////////////////
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    /////////////////
    // From helper...

    // Initialize all the remaining properties
   // initializeBuffers( indices, vertices);
    initGeometry(  heightFactor,  aspectRatio,  nearPlane,  farPlane,   YfieldOfView);

    // initialize uniforms
    initializeUniforms( heightFactor);

    // Set we want to resize the window!
    glfwSetWindowAttrib(win, GLFW_RESIZABLE , GLFW_TRUE);

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(win)) {
        //////////
        // Automatic window size
        int currentWindowWidth;
        int currentWindowHeight;
        glfwGetWindowSize(win, &currentWindowWidth, &currentWindowHeight);
        // cout << currentWindowWidth << endl;
        glViewport(0, 0, currentWindowWidth, currentWindowHeight);


        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
        glNormalPointer(GL_FLOAT,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, tcoord_buffer);
        glTexCoordPointer(2,GL_FLOAT,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexPointer(3,GL_FLOAT,0,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

        glUniform1i(glGetUniformLocation(idProgramShader, "idHeightTexture"), 1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D,idHeightTexture);

        glUniform1i(glGetUniformLocation(idProgramShader, "idJpegTexture"), 1);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D,idJpegTexture);

        glUseProgram(idProgramShader);




        glClearColor(0,0,0,1);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.gaze = glm::normalize(glm::vec3(
          // x
          cos(camera.pitchAngle)*sin(camera.yawAngle),
          // y
          sin(camera.pitchAngle),
          // z
          cos(camera.pitchAngle)*cos(camera.yawAngle)
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

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
