/**
 * @file    App.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "App.h"

#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>      // NOLINT(readability/streams) for files
#include <sstream>
#include <string>
#include <vector>
#include <cassert>


/// Vertex data of a simple triangle, drawn clockwise, followed by its color data
static const float vertexData[] = {
    // les 3 vertex (x,y,z,w) du triangle
    0.0f,  0.65f, 0.0f, 1.0f,
    0.6f, -0.5f,  0.0f, 1.0f,
    -0.6f, -0.5f,  0.0f, 1.0f,
    // les 3 couleurs (r,g,b,a)
    0.8f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.8f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.8f, 1.0f
};


App* App::mpSelf = NULL;


App::App() :
    mLog("App"),
    mProgram(0),
    mAttribPosition(0),
    mAttribColor(0),
    mUniformModelToWorldMatrix(0),
    mUniformWorldToCameraMatrix(0),
    mUniformCameraToClipMatrix(0),
    mVertexBufferObject(0) {
    mpSelf = this;
}
App::~App() {
    mpSelf = NULL;
}


/**
 * @brief Compile a given type shader from the content of a file, and add it to the list
 *
 * @param[in,out]   aShaderList         List of compiled shaders, to be completed by this function
 * @param[in]       aShaderType         Type of shader to be compiled
 * @param[in]       apShaderFilename    Name of the shader file to compile
 *
 * @throw a std::exception in case of error (glutil::CompileLinkException or std::runtime_error)
 */
void App::CompileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const {
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
        mpSelf->mLog.info() << "CompileShader: \"" << apShaderFilename << "\":\n" << e.what();
        throw;  // rethrow to abort program
    }
}

/**
 * @brief compile shaders and link them in a program
 */
void App::initProgram() {
    std::vector<GLuint> shaderList;

    // Compile the shader files (into intermediate compiled object)
    mLog.debug() << "initProgram: compiling shaders...";
    CompileShader(shaderList, GL_VERTEX_SHADER,   "data/ModelWorldCameraClip.vert");
    CompileShader(shaderList, GL_FRAGMENT_SHADER, "data/PassthroughColor.frag");
    // Link them in a program (into the final executable to send to the GPU)
    mLog.debug() << "initProgram: linking program...";
    mProgram = glutil::LinkProgram(shaderList);
    // Now, the intermediate compiled shader can be deleted (the program contain them)
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    // Get location of (vertex) attributes (input streams of (vertex) shader
    mAttribPosition = glGetAttribLocation(mProgram, "position");   // layout(location = 0) in vec4 position;
    mAttribColor    = glGetAttribLocation(mProgram, "color");      // layout(location = 1) in vec4 color;
    // Get location of uniforms - input variables of (vertex) shader
    mUniformModelToWorldMatrix     = glGetUniformLocation(mProgram, "modelToWorldMatrix");
    mUniformWorldToCameraMatrix    = glGetUniformLocation(mProgram, "worldToCameraMatrix");
    mUniformCameraToClipMatrix     = glGetUniformLocation(mProgram, "cameraToClipMatrix");

    // Define a unit matrix for all transformations
    glm::mat4 unityMatrix(1.0f);

    // Set uniform values that are constants (matrix transformation)
    glUseProgram(mProgram);
    glUniformMatrix4fv(mUniformModelToWorldMatrix,  1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUniformMatrix4fv(mUniformWorldToCameraMatrix, 1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUniformMatrix4fv(mUniformCameraToClipMatrix,  1, GL_FALSE, glm::value_ptr(unityMatrix));
    glUseProgram(0);
}

/**
 * @brief init the vertex buffer object with the data of our mesh (triangle)
 */
void App::initVertexBufferObject() {
    mLog.debug() << "initializing vertex buffer objet...";

    glGenBuffers(1, &mVertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Register GLUT callbacks
 */
void App::registerCallbacks() {
    mLog.debug() << "registerCallbacks...";
    glutReshapeFunc        (reshapeCallback);
    glutDisplayFunc        (displayCallback);
    glutKeyboardFunc       (keyboardCallback);
    glutSpecialFunc        (keyboardSpecialCallback);
    glutMouseFunc          (mouseCallback);
    glutMotionFunc         (mouseMotionCallback);
    glutPassiveMotionFunc  (mousePassiveMotionCallback);
    glutMouseWheelFunc     (mouseWheelCallback);
    glutJoystickFunc       (joystickCallback, 10);
}

/**
 * @brief GLUT reshape callback function
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void App::reshapeCallback(int aW, int aH) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "reshapeCallback(" << aW << "," << aH << ")";

    glViewport(0, 0, (GLsizei)aW, (GLsizei)aH);
}

/**
 * @brief GLUT display callback function
 */
void App::displayCallback() {
    assert(NULL != mpSelf);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(mpSelf->mProgram);

    // Bind the vertex buffer, and init vertex position and colors
    glBindBuffer(GL_ARRAY_BUFFER, mpSelf->mVertexBufferObject);
    glEnableVertexAttribArray(mpSelf->mAttribPosition);   // layout(location = 0) in vec4 position;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mpSelf->mAttribColor);      // layout(location = 1) in vec4 color;
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(sizeof(vertexData)/2));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(mpSelf->mAttribPosition);
    glDisableVertexAttribArray(mpSelf->mAttribColor);
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
void App::keyboardCallback(unsigned char aKey, int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.debug() << "keyboardCallback(" << static_cast<int>(aKey) << "='" << aKey
                        << "'," << aX << "," << aY << ")";
    switch (aKey) {
        case 27:   // ESC : EXIT
            glutLeaveMainLoop();
            break;
        case 'w': case 'W':  // QWERTY keyboard disposition
        case 'z': case 'Z':  // AZERTY keyboard disposition
            mpSelf->mLog.info() << "up";
            break;
        case 'a': case 'A':  // QWERTY
        case 'q': case 'Q':  // AZERTY
            mpSelf->mLog.info() << "left";
            break;
        case 's': case 'S':  // QWERTY & AZERTY
            mpSelf->mLog.info() << "down";
            break;
        case 'd': case 'D':  // QWERTY & AZERTY
            mpSelf->mLog.info() << "right";
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
void App::keyboardSpecialCallback(int aKey, int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "keyboardCallback(" << aKey << "," << aX << "," << aY << ")";

    switch (aKey) {
        case GLUT_KEY_UP:
            mpSelf->mLog.info() << "up";
            break;
        case GLUT_KEY_LEFT:
            mpSelf->mLog.info() << "left";
            break;
        case GLUT_KEY_DOWN:
            mpSelf->mLog.info() << "down";
            break;
        case GLUT_KEY_RIGHT:
            mpSelf->mLog.info() << "right";
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
void App::mouseCallback(int aButton, int aState, int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "mouseCallback(" << aButton << "," << ((aState == GLUT_DOWN)?"down":"up")
              << "," << aX << "," << aY << ")";
}

/**
 * @brief GLUT mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void App::mouseMotionCallback(int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "mouseMotionCallback(" << aX << "," << aY << ")";
}

/**
 * @brief GLUT mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void App::mousePassiveMotionCallback(int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "mousePassiveMotionCallback(" << aX << "," << aY << ")";
}

/**
 * @brief GLUT mouse wheel callback function (LOW SENSITIVITY under Windows)
 *
 * @param[in] aNum      Wheel number (0)   
 * @param[in] aDirection Direction 1=up, or -1=down
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void App::mouseWheelCallback(int aNum, int aDirection, int aX, int aY) {
    assert(NULL != mpSelf);

    mpSelf->mLog.info() << "mouseWheelCallback(" << aNum << "," << aDirection << "," << aX << "," << aY << ")";
}

/**
 * @brief GLUT jostick callback function
 *
 * @param[in] aButtonMask   Mask of buttons 
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 * @param[in] aZ        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void App::joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ) {
    assert(NULL != mpSelf);

    static unsigned int lastButtonMask = 0;
    static int lastX = 0;
    static int lastY = 0;
    static int lastZ = 0;
    if ( (lastButtonMask != aButtonMask) || (lastX != aX) || (lastY != aY) || (lastZ != aZ) ) {
        mpSelf->mLog.info() << "joystickCallback(" << aButtonMask << "," << aX << "," << aY << "," << aZ << ")";
        lastButtonMask = aButtonMask;
        lastX = aX;
        lastY = aY;
        lastZ = aZ;
    }
}
