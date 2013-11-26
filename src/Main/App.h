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

#include "Utils/Utils.h"

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...
#include <glutil/MatrixStack.h>

#include <vector>               // std::vector


/**
 * @brief Application managing the lifecycle of glut
 */
class App {
public:
    App();
    ~App();

    inline bool isKeyPressed(unsigned char aKey) const;
    inline bool isSpecialKeyPressed(int aKey) const;

private:
    void init();
    void compileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const;
    void initProgram();
    void initVertexArrayObject();
    void registerCallbacks();
    void uninitVertexArrayObject();

    void checkKeys();
    void up();
    void down();
    void left();
    void right();
    void front();
    void back();
    void rotate(int aDeltaX, int aDeltaY);
    glm::mat4 transform();

    void drawPlane(glutil::MatrixStack& aModelToWorldMatrixStack);
    void drawCube(glutil::MatrixStack& aModelToWorldMatrixStack);

    void calculateFPS();

    // static inline freeglut callback
    static inline void reshapeCallbackStatic(int aW, int aH);
    static inline void displayCallbackStatic();
    static inline void keyboardCallbackStatic(unsigned char aKey, int aX, int aY);
    static inline void keyboardUpCallbackStatic(unsigned char aKey, int aX, int aY);
    static inline void keyboardSpecialCallbackStatic(int aKey, int aX, int aY);
    static inline void keyboardSpecialUpCallbackStatic(int aKey, int aX, int aY);
    static inline void mouseCallbackStatic(int aButton, int aState, int aX, int aY);
    static inline void mouseMotionCallbackStatic(int aX, int aY);
    static inline void mousePassiveMotionCallbackStatic(int aX, int aY);
    static inline void mouseWheelCallbackStatic(int aNum, int aDirection, int aX, int aY);
    static inline void joystickCallbackStatic(unsigned int aButtonMask, int aX, int aY, int aZ);
    // freeglut callback
    void reshapeCallback(int aW, int aH);
    void displayCallback();
    void keyboardCallback(unsigned char aKey, int aX, int aY);
    void keyboardUpCallback(unsigned char aKey, int aX, int aY);
    void keyboardSpecialCallback(int aKey, int aX, int aY);
    void keyboardSpecialUpCallback(int aKey, int aX, int aY);
    void mouseCallback(int aButton, int aState, int aX, int aY);
    void mouseMotionCallback(int aX, int aY);
    void mousePassiveMotionCallback(int aX, int aY);
    void mouseWheelCallback(int aNum, int aDirection, int aX, int aY);
    void joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ);

private:
    static App* _mpSelf;    ///< Static pointer to the unique App instance, for glut static callback

    std::vector<bool>   mKeyPressed;        ///< Current state of regular keyboard keys
    std::vector<bool>   mSpecialKeyPressed; ///< Current state of special keyboard keys

    Log::Logger mLog;       ///< Logger object to output runtime information

    GLuint mProgram;                    ///< OpenGL program containing compiled and linked shaders
    GLuint mPositionAttrib;             ///< Location of the "position" vertex shader attribute (input stream)
    GLuint mColorAttrib;                ///< Location of the "color" vertex shader attribute (input stream)
    GLuint mModelToWorldMatrixUnif;     ///< Location of the "modelToWorldMatrix"  vertex shader uniform input variable
    GLuint mWorldToCameraMatrixUnif;    ///< Location of the "worldToCameraMatrix" vertex shader uniform input variable
    GLuint mCameraToClipMatrixUnif;     ///< Location of the "cameraToClipMatrix"  vertex shader uniform input variable

    GLuint mVertexBufferObject;         ///< Vertex buffer object containing the data of our mesh
    GLuint mIndexBufferObject;          ///< Index buffer object containing the indices of vertices of our mesh
    GLuint mVertexArrayObject;          ///< Vertex array object

    glm::vec3   mModelRotation;         ///< Angles of rotation of the model
    glm::vec3   mModelTranslation;      ///< Vector of translation of the model
};


/**
 * @brief Get the current state of the given regular keyboard key
 *
 * @param[in] aKey  Index of the key
 *
 * @return state of the Key (true if pressed)
 */
inline bool App::isKeyPressed(unsigned char aKey) const {
    return mKeyPressed.at(aKey);
}

/**
 * @brief Get the current state of the given special keyboard key
 *
 * @param[in] aKey  Index of the key
 *
 * @return state of the Key (true if pressed)
 */
inline bool App::isSpecialKeyPressed(int aKey) const {
    return mSpecialKeyPressed.at(aKey);
}


/// @{ Static inline freeglut callbacks, calling
inline void App::reshapeCallbackStatic(int aW, int aH) {
    assert(nullptr != _mpSelf);
    _mpSelf->reshapeCallback(aW, aH);
}
inline void App::displayCallbackStatic() {
    assert(nullptr != _mpSelf);
    _mpSelf->displayCallback();
}
inline void App::keyboardCallbackStatic(unsigned char aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardCallback(aKey, aX, aY);
}
inline void App::keyboardUpCallbackStatic(unsigned char aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardUpCallback(aKey, aX, aY);
}
inline void App::keyboardSpecialCallbackStatic(int aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardSpecialCallback(aKey, aX, aY);
}
inline void App::keyboardSpecialUpCallbackStatic(int aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardSpecialUpCallback(aKey, aX, aY);
}
inline void App::mouseCallbackStatic(int aButton, int aState, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseCallback(aButton, aState, aX, aY);
}
inline void App::mouseMotionCallbackStatic(int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseMotionCallback(aX, aY);
}
inline void App::mousePassiveMotionCallbackStatic(int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mousePassiveMotionCallback(aX, aY);
}
inline void App::mouseWheelCallbackStatic(int aNum, int aDirection, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseWheelCallback(aNum, aDirection, aX, aY);
}
inline void App::joystickCallbackStatic(unsigned int aButtonMask, int aX, int aY, int aZ) {
    assert(nullptr != _mpSelf);
    _mpSelf->joystickCallback(aButtonMask, aX, aY, aZ);
}
