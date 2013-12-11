/**
 * @file    Input.cpp
 * @ingroup opengl-experiments
 * @brief   Management of the GLUT input callbacks
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Input.h"
#include "Main/Renderer.h"
#include "Main/Node.h"

#include "Utils/Time.h"

#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...


// Definition of the static pointer to the unique Input instance
Input* Input::_mpSelf = nullptr;


/**
 * @brief Constructor
 *
 * @param[in] aRenderer Reference to the renderer managing OpenGL drawing
 */
Input::Input(Renderer& aRenderer) :
    mLog("Input"),
    mRenderer(aRenderer),
    mKeyPressed(256, false),
    mSpecialKeyPressed(128, false),
    mFPS(1000000),
    mCubeRotationTimer(16667) { // 16,667ms = 60Hz
    _mpSelf = this;
    init();
}
/**
 * @brief Destructor
 */
Input::~Input() {
    _mpSelf = nullptr;
}


/**
 * @brief Initialization
 */
void Input::init() {
    // 1) Register GLUT Callbacks
    registerCallbacks();
}

/**
 * @brief Register GLUT callbacks
 */
void Input::registerCallbacks() {
    mLog.debug() << "registerCallbacks...";

    glutReshapeFunc(reshapeCallbackStatic);
    glutDisplayFunc(displayCallbackStatic);
    glutKeyboardFunc(keyboardCallbackStatic);
    glutKeyboardUpFunc(keyboardUpCallbackStatic);
    glutSpecialFunc(keyboardSpecialCallbackStatic);
    glutSpecialUpFunc(keyboardSpecialUpCallbackStatic);
    glutMouseFunc(mouseCallbackStatic);
    glutMotionFunc(mouseMotionCallbackStatic);
    glutPassiveMotionFunc(mousePassiveMotionCallbackStatic);
    glutMouseWheelFunc(mouseWheelCallbackStatic);
    glutJoystickFunc(joystickCallbackStatic, 10);
}

/**
* @brief Check current pressed keyboard keys
*/
void Input::checkKeys() {
    if (isKeyPressed(27)) {
        // Exit on Escape
        glutLeaveMainLoop();
    }
    if (   isSpecialKeyPressed(GLUT_KEY_UP)             // Special directional key
        || isKeyPressed('w') || isKeyPressed('W')       // QWERTY keyboard disposition
        || isKeyPressed('z') || isKeyPressed('Z') ) {   // AZERTY keyboard disposition
        // Move up the camera
        mRenderer.move(0.01f * Node::UNIT_Y_UP);
    }
    if (   isSpecialKeyPressed(GLUT_KEY_LEFT)           // Special directional key
        || isKeyPressed('a') || isKeyPressed('A')       // QWERTY keyboard disposition
        || isKeyPressed('q') || isKeyPressed('Q') ) {   // AZERTY keyboard disposition
        // Move the camera to the left
        mRenderer.move(-0.01f * Node::UNIT_X_RIGHT);
    }
    if (   isSpecialKeyPressed(GLUT_KEY_DOWN)           // Special directional key
        || isKeyPressed('s') || isKeyPressed('S') ) {   // QWERTY & AZERTY keyboard disposition
        // Move down the camera
        mRenderer.move(-0.01f * Node::UNIT_Y_UP);
    }
    if (   isSpecialKeyPressed(GLUT_KEY_RIGHT)          // Special directional key
        || isKeyPressed('d') || isKeyPressed('D') ) {   // QWERTY & AZERTY keyboard disposition
        // Move right the camera
        mRenderer.move(0.01f * Node::UNIT_X_RIGHT);
    }
    if (isKeyPressed('x') || isKeyPressed('X')) {
        // Roll left the camera
        mRenderer.roll(-0.01f);
    }
    if (isKeyPressed('c') || isKeyPressed('C')) {
        // Roll right the camera
        mRenderer.roll(0.01f);
    }

    if (isKeyPressed('r') || isKeyPressed('R')) {
        // Move front the model
        mRenderer.modelMove(0.01f * Node::UNIT_Z_FRONT);
    }
    if (isKeyPressed('t') || isKeyPressed('T')) {
        // Move up the model
        mRenderer.modelMove(0.01f * Node::UNIT_Y_UP);
    }
    if (isKeyPressed('y') || isKeyPressed('Y')) {
        // Move back the model
        mRenderer.modelMove(-0.01f * Node::UNIT_Z_FRONT);
    }
    if (isKeyPressed('f') || isKeyPressed('F')) {
        // Move the model to the left
        mRenderer.modelMove(-0.01f * Node::UNIT_X_RIGHT);
    }
    if (isKeyPressed('g') || isKeyPressed('G')) {
        // Move down the model
        mRenderer.modelMove(-0.01f * Node::UNIT_Y_UP);
    }
    if (isKeyPressed('h') || isKeyPressed('H')) {
        // Move right the model
        mRenderer.modelMove(0.01f * Node::UNIT_X_RIGHT);
    }

    if (isKeyPressed('p')) {
        mRenderer.modelPitch(0.001f);
    }
    if (isKeyPressed('m')) {
        mRenderer.modelYaw(0.001f);
    }
    if (isKeyPressed('l')) {
        mRenderer.modelRoll(0.001f);
    }
}

/**
 * @brief GLUT reshape callback function
 *
 *  Called once at the start of the rendering, and then for each window resizing.
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void Input::reshapeCallback(int aW, int aH) {
    mLog.info() << "reshapeCallback(" << aW << "," << aH << ")";

    // Delegate management of OpenGL rendering
    mRenderer.reshape(aW, aH);
}

/**
 * @brief GLUT display callback function
 */
void Input::displayCallback() {
    // mLog.debug() << "displayCallback()";

    // FPS and frame duration calculations
    bool bNewCalculatedFPS = mFPS.calculate();
    if (bNewCalculatedFPS) {
        mLog.notice() << mFPS.getCalculatedFPS() << "fps (avg " << mFPS.getAverageInterFrameUs()/1000.0
                      << "ms, worst " << mFPS.getWorstInterFrameUs()/1000.0 << "ms)";
    }

    // Check current key pressed
    checkKeys();

    // Timer for basic cube animation (rotation around the X axis at 60Hz)
    bool bCubeRotate = mCubeRotationTimer.isTimeElapsed(mFPS.getCurrentFrameTickUs());
    if (bCubeRotate) {
        mRenderer.modelYaw(0.005f);
    }

    // Delegate management of OpenGL rendering
    mRenderer.display();
}

/**
 * @brief GLUT keyboard key pressed callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::keyboardCallback(unsigned char aKey, int aX, int aY) {
    mLog.debug() << "keyboardCallback(" << static_cast<int>(aKey) << "='" << aKey
                        << "'," << aX << "," << aY << ")";

    mKeyPressed[aKey] = true;
}

/**
 * @brief GLUT keyboard key released callback function
 *
 * @param[in] aKey  ASCII code of the key released
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::keyboardUpCallback(unsigned char aKey, int aX, int aY) {
    mLog.debug() << "keyboardUpCallback(" << static_cast<int>(aKey) << "='" << aKey
                        << "'," << aX << "," << aY << ")";

    mKeyPressed[aKey] = false;
}

/**
 * @brief GLUT keyboard special key pressed callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::keyboardSpecialCallback(int aKey, int aX, int aY) {
    mLog.debug() << "keyboardCallback(" << aKey << "," << aX << "," << aY << ")";

    mSpecialKeyPressed[aKey] = true;
}

/**
 * @brief GLUT keyboard special key release callback function
 *
 * @param[in] aKey  ASCII code of the key release
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::keyboardSpecialUpCallback(int aKey, int aX, int aY) {
    mLog.debug() << "keyboardSpecialUpCallback(" << aKey << "," << aX << "," << aY << ")";

    mSpecialKeyPressed[aKey] = false;
}

/**
 * @brief GLUT mouse button click callback function
 *
 * @param[in] aButton   Mouse button
 * @param[in] aState    State of the button
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::mouseCallback(int aButton, int aState, int aX, int aY) {
    mLog.debug() << "mouseCallback(" << aButton << "," << ((aState == GLUT_DOWN)?"down":"up")
              << "," << aX << "," << aY << ")";

    // Detect Mouse Wheel under X (Linux Ubuntu 12.10)
    if (3 == aButton) {
        if (GLUT_DOWN == aState) {
            mouseWheelCallback(0, -1, aX, aY);
        }
    } else if (4 == aButton) {
        if (GLUT_DOWN == aState) {
            mouseWheelCallback(0, 1, aX, aY);
        }
    }
}


/// Last known position of the mouse for mouse (passive) motion functions
static glm::detail::tvec2<int> _lastMousePos;

/**
 * @brief GLUT mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */    
void Input::mouseMotionCallback(int aX, int aY) {
    mLog.debug() << "mouseMotionCallback(" << aX << "," << aY << ")";

    if (aY != _lastMousePos.y) {
        mRenderer.pitch(0.01f * (aY - _lastMousePos.y));
        _lastMousePos.y = aY;
    }
    if (aX != _lastMousePos.x) {
        mRenderer.yaw(0.01f * (aX - _lastMousePos.x));
        _lastMousePos.x = aX;
    }
}

/**
 * @brief GLUT mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */    
void Input::mousePassiveMotionCallback(int aX, int aY) {
    mLog.debug() << "mousePassiveMotionCallback(" << aX << "," << aY << ")";

    _lastMousePos.x = aX;
    _lastMousePos.y = aY;
}

/**
 * @brief GLUT mouse wheel callback function (LOW SENSITIVITY under Windows)
 *
 * @param[in] aNum  Wheel number (0)
 * @param[in] aDirection Direction 1=up, or -1=down
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void Input::mouseWheelCallback(int aNum, int aDirection, int aX, int aY) {
    mLog.debug() << "mouseWheelCallback(" << aNum << "," << aDirection << "," << aX << "," << aY << ")";

    if (0 > aDirection) {
        // Update Z translation value to the front
        mRenderer.move(0.1f * Node::UNIT_Z_FRONT);;
    } else {
        // Update Z translation value to the back
        mRenderer.move(-0.1f * Node::UNIT_Z_FRONT);
    }
    // transform() will re-calculate the model to world transformations matrix in next displayCallback()
}

/**
 * @brief GLUT joystick callback function
 *
 * @param[in] aButtonMask Mask of buttons
 * @param[in] aX    X coord of the joystick (between -1000 and 1000)
 * @param[in] aY    Y coord of the joystick (between -1000 and 1000)
 * @param[in] aZ    Z coord of the joystick (between -1000 and 1000)
 */
void Input::joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ) {
    static unsigned int             _lastButtonMask = 0;
    static glm::detail::tvec3<int>  _lastJoystickPos;
    if (   (_lastButtonMask != aButtonMask)
        || (_lastJoystickPos.x != aX) || (_lastJoystickPos.y != aY) || (_lastJoystickPos.z != aZ) ) {
        mLog.debug() << "joystickCallback(" << aButtonMask << "," << aX << "," << aY << "," << aZ << ")";
        _lastButtonMask = aButtonMask;
        _lastJoystickPos.x = aX;
        _lastJoystickPos.y = aY;
        _lastJoystickPos.z = aZ;
    }
}
