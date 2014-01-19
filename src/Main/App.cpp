/**
 * @file    App.cpp
 * @ingroup Main
 * @brief   Application managing the lifecycle of GLFW window and inputs.
 *
 * Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/App.h"
#include "Utils/FPS.h"

#include <cassert>

/**
 * @brief Constructor
 *
 * @param[in] apWindow  Pointer to the GLFW window
 */
App::App(GLFWwindow* apWindow) :
    mLog("App"),
    mpWindow(apWindow) {
}
/**
 * @brief Destructor
 */
App::~App() {
}

/**
 * @brief Render loop
 */
void App::loop() {
    Utils::FPS FPS(1.0f);   // Calculate FPS once per second
    int width = 0;
    int height = 0;

    assert(nullptr != mpWindow);

    glfwGetFramebufferSize(mpWindow, &width, &height);
    mLog.info() << "framebuffer (" << width << " x " << height << ")";

    // Call the renderer display method
    mRenderer.reshape(width, height);

    mLog.info() << "Loop";
    while (!glfwWindowShouldClose(mpWindow)) {
        // FPS and frame duration calculations
        bool bNewCalculatedFPS = FPS.start(static_cast<float>(glfwGetTime()));
        if (bNewCalculatedFPS) {
            mLog.notice() << std::fixed << std::setprecision(1)
                          << FPS.getCalculatedFPS() << "fps (avg " << FPS.getAverageInterFrame()*1000.0f << "ms, worst "
                          << FPS.getWorstInterFrame()*1000.0f << "ms) RenderTime "
                          << FPS.getLastRenderTime()*1000.0f << "ms ("
                          << FPS.getLastRenderTime()*100.0f/FPS.getElapsedTime() << "%)";
        }

        // Check current key pressed
        checkKeys();

        // Do all movements based on Node speed
        mRenderer.move(FPS.getElapsedTime());

        // Get orientation of the Oculus Head Mounted Display
        glm::fquat orientation = mOculusHMD.getOrientation();
        mRenderer.setCameraOrientation(orientation);

        // Call the renderer display method
        mRenderer.display();

        FPS.end(static_cast<float>(glfwGetTime()));

        // Swap back & front buffers
        glfwSwapBuffers(mpWindow);

        // Process events
        glfwPollEvents();
    }
}


/**
* @brief Check current pressed keyboard keys
*/
void App::checkKeys() {
    assert(nullptr != mpWindow);

    if (isKeyPressed(GLFW_KEY_ESCAPE)) {
        // Exit on Escape
        glfwSetWindowShouldClose(mpWindow, GL_TRUE);
    }

    if (   isKeyPressed(GLFW_KEY_UP)    // Special directional key
        || isKeyPressed(GLFW_KEY_W)     // QWERTY keyboard disposition
        || isKeyPressed(GLFW_KEY_Z) ) { // AZERTY keyboard disposition
        // Move up the camera
        mRenderer.move(0.01f * Node::UNIT_Y_UP);
    }
    if (   isKeyPressed(GLFW_KEY_LEFT)  // Special directional key
        || isKeyPressed(GLFW_KEY_A)     // QWERTY keyboard disposition
        || isKeyPressed(GLFW_KEY_Q) ) { // AWERTY keyboard disposition
        // Move the camera to the left
        mRenderer.move(-0.01f * Node::UNIT_X_RIGHT);
    }
    if (   isKeyPressed(GLFW_KEY_DOWN)  // Special directional key
        || isKeyPressed(GLFW_KEY_S) ) { // QWERTY & AZERTY keyboard disposition
        // Move down the camera
        mRenderer.move(-0.01f * Node::UNIT_Y_UP);
    }
    if (   isKeyPressed(GLFW_KEY_RIGHT) // Special directional key
        || isKeyPressed(GLFW_KEY_D) ) { // QWERTY & AZERTY keyboard disposition
        // Move right the camera
        mRenderer.move(0.01f * Node::UNIT_X_RIGHT);
    }

    if (isKeyPressed(GLFW_KEY_SPACE)) {
        // Space to reset orientation
        mOculusHMD.resetOrientation();
    }
    if (isKeyPressed(GLFW_KEY_KP_ADD)) {
        // + to increase Occulus HMD predication lookahead by one second (default is 30ms)
        mOculusHMD.incrPrediction(1);
    }
    if (isKeyPressed(GLFW_KEY_KP_SUBTRACT)) {
        // - to decrease Occulus HMD predication lookahead by one second (default is 30ms)
        mOculusHMD.incrPrediction(-1);
    }
    if (isKeyPressed(GLFW_KEY_KP_MULTIPLY)) {
        // * to increase Occulus HMD screen center offset
        mRenderer.incrScreenCenterOffset(0.001f);
    }
    if (isKeyPressed(GLFW_KEY_KP_DIVIDE)) {
        // / to decrease Occulus HMD screen center offset
        mRenderer.incrScreenCenterOffset(-0.001f);
    }

    if (isKeyPressed(GLFW_KEY_R)) {
        // Move front the model
        mRenderer.modelMove(0.01f * Node::UNIT_Z_FRONT);
    }
    if (isKeyPressed(GLFW_KEY_T)) {
        // Move up the model
        mRenderer.modelMove(0.01f * Node::UNIT_Y_UP);
    }
    if (isKeyPressed(GLFW_KEY_Y)) {
        // Move back the model
        mRenderer.modelMove(-0.01f * Node::UNIT_Z_FRONT);
    }
    if (isKeyPressed(GLFW_KEY_F)) {
        // Move the model to the left
        mRenderer.modelMove(-0.01f * Node::UNIT_X_RIGHT);
    }
    if (isKeyPressed(GLFW_KEY_G)) {
        // Move down the model
        mRenderer.modelMove(-0.01f * Node::UNIT_Y_UP);
    }
    if (isKeyPressed(GLFW_KEY_H)) {
        // Move right the model
        mRenderer.modelMove(0.01f * Node::UNIT_X_RIGHT);
    }

    if (isKeyPressed(GLFW_KEY_P)) {
        mRenderer.modelPitch(0.001f);
    }
    if (isKeyPressed(GLFW_KEY_M)) {
        mRenderer.modelYaw(0.001f);
    }
    if (isKeyPressed(GLFW_KEY_L)) {
        mRenderer.modelRoll(0.001f);
    }
}

/// @todo App: Restore Mouse and Joystick movements

/**
 * @brief GLFW mouse button click callback function
 *
 * @param[in] aButton   Mouse button
 * @param[in] aState    State of the button
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 *
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

/// 2d screen coordinates
struct coord2d {
    int x;  ///< x integer coordinate
    int y;  ///< y integer coordinate
};
/// 3d coordinates
struct coord3d {
    int x;  ///< x integer coordinate
    int y;  ///< y integer coordinate
    int z;  ///< z integer coordinate
};

/// Last known position of the mouse for mouse (passive) motion functions
static coord2d _lastMousePos;

/**
 * @brief GLFW mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 *
void Input::mouseMotionCallback(int aX, int aY) {
    mLog.debug() << "mouseMotionCallback(" << aX << "," << aY << ")";
}

/**
 * @brief GLFW mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 *
void Input::mousePassiveMotionCallback(int aX, int aY) {
    mLog.debug() << "mousePassiveMotionCallback(" << aX << "," << aY << ")";

    _lastMousePos.x = aX;
    _lastMousePos.y = aY;
}

/**
 * @brief GLFW mouse wheel callback function (LOW SENSITIVITY under Windows)
 *
 * @param[in] aNum  Wheel number (0)
 * @param[in] aDirection Direction 1=up, or -1=down
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 *
void Input::mouseWheelCallback(int aNum, int aDirection, int aX, int aY) {
    mLog.debug() << "mouseWheelCallback(" << aNum << "," << aDirection << "," << aX << "," << aY << ")";

    if (0 > aDirection) {
        // Update Z translation value to the front
        mRenderer.move(1.0f * Node::UNIT_Z_FRONT);;
    } else {
        // Update Z translation value to the back
        mRenderer.move(-1.0f * Node::UNIT_Z_FRONT);
    }
    // transform() will re-calculate the model to world transformations matrix in next displayCallback()
}

/**
 * @brief GLFW joystick callback function
 *
 * @param[in] aButtonMask Mask of buttons
 * @param[in] aX    X coord of the joystick (between -1000 and 1000)
 * @param[in] aY    Y coord of the joystick (between -1000 and 1000)
 * @param[in] aZ    Z coord of the joystick (between -1000 and 1000)
 *
void Input::joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ) {
    static unsigned int _lastButtonMask = 0;
    static coord3d      _lastJoystickPos;
    if (   (_lastButtonMask != aButtonMask)
        || (_lastJoystickPos.x != aX) || (_lastJoystickPos.y != aY) || (_lastJoystickPos.z != aZ) ) {
        mLog.debug() << "joystickCallback(" << aButtonMask << "," << aX << "," << aY << "," << aZ << ")";
        _lastButtonMask = aButtonMask;
        _lastJoystickPos.x = aX;
        _lastJoystickPos.y = aY;
        _lastJoystickPos.z = aZ;
    }
}
*/
