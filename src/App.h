/**
 * @file    App.h
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "LoggerCpp/LoggerCpp.h"

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>

#include <vector>


/**
 * @brief TODO
 */
class App {
public:
    App();
    ~App();

    void init();

    // glut static callback
    static void reshapeCallback(int aW, int aH);
    static void displayCallback();
    static void keyboardCallback(unsigned char aKey, int aX, int aY);
    static void keyboardSpecialCallback(int aKey, int aX, int aY);
    static void mouseCallback(int aButton, int aState, int aX, int aY);
    static void mouseMotionCallback(int aX, int aY);
    static void mousePassiveMotionCallback(int aX, int aY);
    static void mouseWheelCallback(int aNum, int aDirection, int aX, int aY);
    static void joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ);

private:
    void CompileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const;
    void initProgram();
    void initVertexBufferObject();
    void initVertexArrayObject();
    void registerCallbacks();

private:
    static App* mpSelf; ///< Static pointer to the unique App instance, for glut static callback
    Log::Logger mLog;   ///< Logger object to output runtime information

    GLuint mProgram;                    ///< OpenGL program containing compiled and linked shaders
    GLuint mAttribPosition;             ///< Location of the "position" vertex shader attribute (input stream)
    GLuint mAttribColor;                ///< Location of the "color" vertex shader attribute (input stream)
    GLuint mUniformModelToWorldMatrix;  ///< Location of the "modelToWorldMatrix"  vertex shader uniform input variable
    GLuint mUniformWorldToCameraMatrix; ///< Location of the "worldToCameraMatrix" vertex shader uniform input variable
    GLuint mUniformCameraToClipMatrix;  ///< Location of the "cameraToClipMatrix"  vertex shader uniform input variable

    GLuint mVertexBufferObject;         ///< Vertex buffer object containing the data of our mesh (triangle)
    GLuint mVertexArrayObject;          ///< Vertex array object
};
