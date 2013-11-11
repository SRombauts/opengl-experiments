/**
 * @file    main.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>
#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "LoggerCpp/LoggerCpp.h"

#include <fstream>      // NOLINT(readability/streams) for files
#include <sstream>
#include <string>
#include <vector>

/// Vertex of a simple triangle, drawn clockwise, followed by its color data
static const float g_vertexData[] = {
    // les 3 vertex (x,y,z,w) du triangle
    0.0f,  0.65f, 0.0f, 1.0f,
    0.6f, -0.5f,  0.0f, 1.0f,
    -0.6f, -0.5f,  0.0f, 1.0f,
    // les 3 couleurs (r,g,b,a)
    0.8f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.8f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.8f, 1.0f
};

GLuint g_program;
GLuint g_attribPosition;
GLuint g_attribColor;
GLuint g_uniformModelToWorldMatrix;
GLuint g_uniformWorldToCameraMatrix;
GLuint g_uniformCameraToClipMatrix;

GLuint g_vertexBufferObject;


/**
 * @brief Compile a given type shader from the content of a file, and add it to the list
 *
 * @param[in,out]   aShaderList         List of compiled shaders, to be completed by this function
 * @param[in]       aShaderType         Type of shader to be compiled
 * @param[in]       apShaderFilename    Name of the shader file to compile
 *
 * @throw a std::exception in case of error (glutil::CompileLinkException or std::runtime_error)
 */
void CompileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename)  {
    Log::Logger log("CompileShader");
    try {
        std::ifstream ifShader(apShaderFilename);
        if (ifShader.is_open()) {
            std::ostringstream isShader;
            isShader << ifShader.rdbuf();
            log.debug() << "CompileShader: compiling \"" << apShaderFilename << "\"...";
            aShaderList.push_back(glutil::CompileShader(aShaderType, isShader.str()));
        } else {
            log.critic() << "CompileShader: unavailable file \"" << apShaderFilename << "\"";
            throw std::runtime_error(std::string("CompileShader: unavailable file ") + apShaderFilename);
        }
    }
    catch(glutil::CompileLinkException& e) {
        std::cout << "CompileShader: \"" << apShaderFilename << "\":\n" << e.what() << std::endl;
        throw;  // rethrow to abort program
    }
}

/**
 * @brief compile shaders and link them in a program
 */
void initProgram(void) {
    Log::Logger log("initProgram");
    std::vector<GLuint> shaderList;
    // Compile the shader files (into intermediate compiled object)
    CompileShader(shaderList, GL_VERTEX_SHADER,   "data/ModelWorldCameraClip.vert");
    CompileShader(shaderList, GL_FRAGMENT_SHADER, "data/PassthroughColor.frag");
    // Link them in a program (into the final executable to send to the GPU)
    log.debug() << "initProgram: linking program...";
    g_program = glutil::LinkProgram(shaderList);
    // Now, the intermediate compiled shader can be deleted (the program contain them)
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    // Get location of (vertex) attributes - input streams of (vertex) shader
    g_attribPosition    = glGetAttribLocation(g_program, "position");   // layout(location = 0) in vec4 position;
    g_attribColor       = glGetAttribLocation(g_program, "color");      // layout(location = 1) in vec4 color;
    // Get id of uniforms - input variables of shader
    g_uniformModelToWorldMatrix     = glGetUniformLocation(g_program, "modelToWorldMatrix");
    g_uniformWorldToCameraMatrix    = glGetUniformLocation(g_program, "worldToCameraMatrix");
    g_uniformCameraToClipMatrix     = glGetUniformLocation(g_program, "cameraToClipMatrix");

    // Define a unit matrix for all transformations
    glm::mat4 unityMatrix(1.0f);

    // Set uniform values that are constants (matrix transformation)
    glUseProgram(g_program);
    glUniformMatrix4fv(g_uniformModelToWorldMatrix,  1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUniformMatrix4fv(g_uniformWorldToCameraMatrix, 1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUniformMatrix4fv(g_uniformCameraToClipMatrix,  1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUseProgram(0);
}

/**
 * @brief init the vertex buffer
 */
void initVertexBufferObject(void) {
    glGenBuffers(1, &g_vertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexData), g_vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief GLUT reshape callback function
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void reshapeCallback(int aW, int aH) {
    std::cout << "reshapeCallback(" << aW << "," << aH << ")" << std::endl;

    glViewport(0, 0, (GLsizei)aW, (GLsizei)aH);
}

/**
 * @brief GLUT display callback function
 */
void displayCallback(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(g_program);

    // Bind the vertex buffer, and init vertex position and colors
    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);
    glEnableVertexAttribArray(g_attribPosition);   // layout(location = 0) in vec4 position;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(g_attribColor);      // layout(location = 1) in vec4 color;
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(sizeof(g_vertexData)/2));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(g_attribPosition);
    glDisableVertexAttribArray(g_attribColor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

/**
 * @brief GLUT keyboard callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void keyboardCallback(unsigned char aKey, int aX, int aY) {
    std::cout << "keyboardCallback(" << static_cast<int>(aKey) << "='" << aKey
              << "'," << aX << "," << aY << ")" << std::endl;
    switch (aKey) {
        case 27:   // ESC : EXIT
            glutLeaveMainLoop();
            break;
        case 'w': case 'W':  // QWERTY keyboard disposition
        case 'z': case 'Z':  // AZERTY keyboard disposition
            std::cout << "up" << std::endl;
            break;
        case 'a': case 'A':  // QWERTY
        case 'q': case 'Q':  // AZERTY
            std::cout << "left" << std::endl;
            break;
        case 's': case 'S':  // QWERTY & AZERTY
            std::cout << "down" << std::endl;
            break;
        case 'd': case 'D':  // QWERTY & AZERTY
            std::cout << "right" << std::endl;
            break;
        default:
            break;
    }
}

/**
 * @brief GLUT special kyes keyboard callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void keyboardSpecialCallback(int aKey, int aX, int aY) {
    std::cout << "keyboardCallback(" << aKey << "," << aX << "," << aY << ")" << std::endl;
    switch (aKey) {
        case GLUT_KEY_UP:
            std::cout << "up" << std::endl;
            break;
        case GLUT_KEY_LEFT:
            std::cout << "left" << std::endl;
            break;
        case GLUT_KEY_DOWN:
            std::cout << "down" << std::endl;
            break;
        case GLUT_KEY_RIGHT:
            std::cout << "right" << std::endl;
            break;
        default:
            break;
    }
}

/**
 * @brief GLUT mouse button click callback function
 *
 * @param[in] aButton   Mouse button
 * @param[in] aState    State of the button
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void mouseCallback(int aButton, int aState, int aX, int aY) {
    std::cout << "mouseCallback(" << aButton << "," << ((aState == GLUT_DOWN)?"down":"up")
              << "," << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void mouseMotionCallback(int aX, int aY) {
    std::cout << "mouseMotionCallback(" << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void mousePassiveMotionCallback(int aX, int aY) {
    std::cout << "mousePassiveMotionCallback(" << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT mouse wheel callback function (LOW SENSITIVITY under Windows)
 *
 * @param[in] aNum      Wheel number (0)   
 * @param[in] aDirection Direction 1=up, or -1=down
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void mouseWheelCallback(int aNum, int aDirection, int aX, int aY) {
    std::cout << "mouseWheelCallback(" << aNum << "," << aDirection << "," << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT jostick callback function
 *
 * @param[in] aButtonMask   Mask of buttons 
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 * @param[in] aZ        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ) {
    static unsigned int lastButtonMask = 0;
    static int lastX = 0;
    static int lastY = 0;
    static int lastZ = 0;
    if ( (lastButtonMask != aButtonMask) || (lastX != aX) || (lastY != aY) || (lastZ != aZ) ) {
        std::cout << "joystickCallback(" << aButtonMask << "," << aX << "," << aY << "," << aZ << ")" << std::endl;
        lastButtonMask = aButtonMask;
        lastX = aX;
        lastY = aY;
        lastZ = aZ;
    }
}

/**
 * @brief Main method - main entry point of application
 *
 *  freeglut does the window creation work for us regardless of the platform.
 */
int main(int argc, char** argv) {
    // Configure the default severity Level of new Channel objects
#ifndef NDEBUG
    Log::Manager::setDefaultLevel(Log::Log::eDebug);
#else
    Log::Manager::setDefaultLevel(Log::Log::eNotice);
#endif

    // Configure the Output objects
    Log::Config::Vector configList;
    Log::Config::addOutput(configList, "OutputConsole");
    Log::Config::addOutput(configList, "OutputFile");
    Log::Config::setOption(configList, "filename",          "log.txt");
    Log::Config::setOption(configList, "filename_old",      "log.old.txt");
    Log::Config::setOption(configList, "max_startup_size",  "0");
    Log::Config::setOption(configList, "max_size",          "10000");
#ifdef WIN32
    Log::Config::addOutput(configList, "OutputDebug");
#endif
    Log::Manager::configure(configList);
    Log::Logger log("main");

    log.info() << "freeglut starting...";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);

    log.notice() << "creating window...";
    int window = glutCreateWindow("OpenGL Experiments");

    log.debug() << "loading functions...";
    glload::LoadFunctions();
    log.notice() << "OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion();

    if (0 == glload::IsVersionGEQ(3, 3)) {
        log.error() << "You must have at least OpenGL 3.3";
        glutDestroyWindow(window);
        return 0;
    }

    // 1) compile shaders and link them in a program
    log.debug() << "initializing program...";
    initProgram();
    // 2) init the vertex buffer
    log.debug() << "initializing vertex buffer objet...";
    initVertexBufferObject();

    // Register GLUT Callbacks
    glutReshapeFunc(reshapeCallback);
    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(keyboardSpecialCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mousePassiveMotionCallback);
    glutMouseWheelFunc(mouseWheelCallback);
    glutJoystickFunc(joystickCallback, 10);

    // Main Loop
    log.notice() << "main loop starting...";
    glutMainLoop();
    log.notice() << "bye...";

    return 0;
}
