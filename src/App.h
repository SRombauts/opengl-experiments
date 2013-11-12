/**
 * @file    App.h
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "LoggerCpp/LoggerCpp.h"

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>


/**
 * @brief TODO
 */
class App {
public:
    App();
    ~App();

    void initProgram();
    void initVertexBufferObject();
    void registerCallbacks();

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

private:
    static App* mpSelf;
    Log::Logger mLog;

    GLuint mProgram;
    GLuint mAttribPosition;
    GLuint mAttribColor;
    GLuint mUniformModelToWorldMatrix;
    GLuint mUniformWorldToCameraMatrix;
    GLuint mUniformCameraToClipMatrix;

    GLuint mVertexBufferObject;
};
