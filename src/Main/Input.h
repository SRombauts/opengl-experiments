/**
 * @file    Input.h
 * @ingroup opengl-experiments
 * @brief   Management of the GLUT input callbacks
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "LoggerCpp/LoggerCpp.h"

#include "Utils/Utils.h"
#include "Utils/FPS.h"
#include "Utils/Timer.h"

#include <vector>
#include <cassert>


// Manage OpenGL rendering
class Renderer;


/**
 * @brief Management of the GLUT input callbacks
 */
class Input {
public:
    explicit Input(Renderer& aRenderer);
    ~Input(); // not virtual because no virtual methods and class not derived

    inline bool isKeyPressed(unsigned char aKey) const;
    inline bool isSpecialKeyPressed(int aKey) const;

private:
    // Initialization
    void init();
    void registerCallbacks();

    // Check current pressed keyboard keys at the beginning of each frame
    void checkKeys();

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
    static Input*       _mpSelf;                ///< Static pointer to the Input instance, for glut static callback

    Log::Logger         mLog;                   ///< Logger object to output runtime information

    Renderer&           mRenderer;              ///< Reference to the renderer managing OpenGL drawing

    std::vector<bool>   mKeyPressed;            ///< Current state of regular keyboard keys
    std::vector<bool>   mSpecialKeyPressed;     ///< Current state of special keyboard keys

    Utils::FPS          mFPS;                   ///< FPS and inter-frame times calculation
    Utils::Timer        mCubeRotationTimer;     ///< Timer to animate the cube

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Input);
};


/**
 * @brief Get the current state of the given regular keyboard key
 *
 * @param[in] aKey  Index of the key
 *
 * @return state of the Key (true if pressed)
 */
inline bool Input::isKeyPressed(unsigned char aKey) const {
    return mKeyPressed.at(aKey);
}

/**
 * @brief Get the current state of the given special keyboard key
 *
 * @param[in] aKey  Index of the key
 *
 * @return state of the Key (true if pressed)
 */
inline bool Input::isSpecialKeyPressed(int aKey) const {
    return mSpecialKeyPressed.at(aKey);
}


/// @{ Static inline freeglut callbacks, calling
inline void Input::reshapeCallbackStatic(int aW, int aH) {
    assert(nullptr != _mpSelf);
    _mpSelf->reshapeCallback(aW, aH);
}
inline void Input::displayCallbackStatic() {
    assert(nullptr != _mpSelf);
    _mpSelf->displayCallback();
}
inline void Input::keyboardCallbackStatic(unsigned char aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardCallback(aKey, aX, aY);
}
inline void Input::keyboardUpCallbackStatic(unsigned char aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardUpCallback(aKey, aX, aY);
}
inline void Input::keyboardSpecialCallbackStatic(int aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardSpecialCallback(aKey, aX, aY);
}
inline void Input::keyboardSpecialUpCallbackStatic(int aKey, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->keyboardSpecialUpCallback(aKey, aX, aY);
}
inline void Input::mouseCallbackStatic(int aButton, int aState, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseCallback(aButton, aState, aX, aY);
}
inline void Input::mouseMotionCallbackStatic(int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseMotionCallback(aX, aY);
}
inline void Input::mousePassiveMotionCallbackStatic(int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mousePassiveMotionCallback(aX, aY);
}
inline void Input::mouseWheelCallbackStatic(int aNum, int aDirection, int aX, int aY) {
    assert(nullptr != _mpSelf);
    _mpSelf->mouseWheelCallback(aNum, aDirection, aX, aY);
}
inline void Input::joystickCallbackStatic(unsigned int aButtonMask, int aX, int aY, int aZ) {
    assert(nullptr != _mpSelf);
    _mpSelf->joystickCallback(aButtonMask, aX, aY, aZ);
}
