/**
 * @file    App.h
 * @ingroup Main
 * @brief   Application managing the lifecycle of GLFW window and inputs.
 *
 * Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

// NOTE: Needs to be included before any other gl/glfw/freeglut header
#include <glload/gl_3_3.h>      // GLuint, GLenum, and OpenGL 3.3 core function APIs
#include <GLFW/glfw3.h>

#include "Main/Renderer.h"

#include "Utils/Utils.h"

#include "Main/OculusHMD.h"

/**
 * @brief Application managing the lifecycle of GLFW window and inputs.
 */
class App {
public:
    explicit App(GLFWwindow* apWindow);
    ~App();

    // Application main loop
    void loop();

private:
    // Check current pressed keyboard keys at the beginning of each frame
    void checkKeys();

    inline bool isKeyPressed(int aKey) const;

private:
    Log::Logger mLog;       ///< Logger object to output runtime information

    Renderer    mRenderer;  ///< Manage OpenGL rendering
    OculusHMD   mOculusHMD; ///< Manage Oculus Head Mounted Display inputs
    GLFWwindow* mpWindow;   ///< Pointer to the GLFW window

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(App);
};

/**
 * @brief Get the current state of the given keyboard key
 *
 * @param[in] aKey  GLFW index of the key
 *
 * @return state of the Key (true if pressed)
 */
inline bool App::isKeyPressed(int aKey) const {
    return (GLFW_PRESS == glfwGetKey(mpWindow, aKey));
}
