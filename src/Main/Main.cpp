/**
 * @file    Main.cpp
 * @ingroup Main
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
/**
 * @defgroup  Main Main
 * @brief     Simple experiments in C++ using the Unofficial OpenGL SDK
 */
/**
 * @dir     Main Main
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 * @ingroup Main
 */

#include "Main/App.h"

// NOTE: OpengGL 3.3 pointers to core function APIs need to be loaded before any GL function is used
#include <glload/gl_load.hpp>   // LoadFunctions() Load pointers for function APIs declared in <glload/gl_3_3.h>s
#include <GLFW/glfw3.h>

#include <cassert>

/**
 * @brief GLFW error callback
 *
 * @param[in] aError        The GLFW error code
 * @param[in] apDescription A UTF-8 encoded string describing the error.
 */
static void glfw_error_callback(int aError, const char* apDescription) {
    static Log::Logger log("glfw_error");
    log.error() << "glfw error(" << aError << "): '" << apDescription << "'";
}

/**
 * @brief Main method - main entry point of application
 *
 *  glfw does the window creation work for us regardless of the platform.
 *
 * @param[in] argc   Number or argument given on the command line (starting with 1, the executable itself)
 * @param[in] argv   Array of pointers of strings containing the arguments
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char** argv) {
    int         retVal = EXIT_SUCCESS;
    GLFWwindow* window;

    // Configure the default severity Level of new Channel objects
#ifndef NDEBUG
    Log::Manager::setDefaultLevel(Log::Log::eDebug);
#else
    Log::Manager::setDefaultLevel(Log::Log::eNotice);
#endif

    // Configure the Output objects
    /// @todo Use a configuration file for LoggerCpp!
    Log::Config::Vector configList;
#if 0
    Log::Config::addOutput(configList, "OutputFile");
    Log::Config::setOption(configList, "filename",          "log.txt");
    Log::Config::setOption(configList, "filename_old",      "log.old.txt");
    Log::Config::setOption(configList, "max_startup_size",  "0");
    Log::Config::setOption(configList, "max_size",          "10000");
#endif
    Log::Config::addOutput(configList, "OutputConsole");
#if defined(WIN32) && !defined(NDEBUG)
    Log::Config::addOutput(configList, "OutputDebug");
#endif
    Log::Manager::configure(configList);
    Log::Logger log("main");

    log.info() << "glfw starting...";
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
    log.critic() << "glfw init failed!";
        return EXIT_FAILURE;
    }

    // NOTE: glfw always use double buffering, as well as alph blending, depth buffer and stencil buffer
    // Configure number of samples of multisampling : 4x is the default value, 8x/16x are not guaranted in all hardware
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Ask for SRGB linear to gama conversion buffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Ask for vertical synch (not working by default under Windows Vista/7/8)
    // glfwSwapInterval(1);

    // get the list of monitors (at least one monitor shall be detected)
    int monitorCount = 0;
    GLFWmonitor** pMonitorList = glfwGetMonitors(&monitorCount);
    assert(0 < monitorCount);
    // if there is only one monitor, use it, but if there is more than one, use the last one
    // (Oculus Rift is expected to be the second monitor, extending the primary one
    GLFWmonitor* pMonitor = pMonitorList[monitorCount-1];

    // get the current screen resolution and colour depth of the choosen monitor
    const GLFWvidmode* pCurrentVideoMod = glfwGetVideoMode(pMonitor);
    int width = pCurrentVideoMod->width;
    int height = pCurrentVideoMod->height;
    log.info() << "fullscreen (" << width << " x " << height << ")";

    // Open a fullscreen window on the last monitor
    window = glfwCreateWindow(width, height, "Simple example", pMonitor, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // NOTE: OpengGL 3.3  core function API pointers need to be loaded before any of thoses function is used
    log.debug() << "loading OpenGL functions into the window context...";
    glload::LoadFunctions();
    log.notice() << "OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion();

    if (0 == glload::IsVersionGEQ(3, 3)) {
        log.error() << "You must have at least OpenGL 3.3";
#ifndef OPENGL_EXPERIMENT_FULLSCREEN_GAME_MODE
#endif // OPENGL_EXPERIMENT_FULLSCREEN_GAME_MODE
    } else {
        try {
            // Create and initialize the application
            // and try to detect an Oculus Rift Head Mounted Display
            App app(window);

            // Application main Loop
            log.notice() << "main loop starting...";
            app.loop();
            log.notice() << "main loop exited";

            // Destructor of App will release memory
        } catch (std::exception& e) {
            log.critic() << "Exception '" << e.what() << "'";
            retVal = EXIT_FAILURE;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    log.notice() << "bye...";

    return retVal;
}
