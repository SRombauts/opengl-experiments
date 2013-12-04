/**
 * @file    Main.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/App.h"

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <GL/freeglut.h>


/**
 * @brief Main method - main entry point of application
 *
 *  freeglut does the window creation work for us regardless of the platform.
 *
 * @param[in] argc   Number or argument given on the command line (starting with 1, the executable itself)
 * @param[in] argv   Array of pointers of strings containing the arguments
 *
 * @return 0
 */
int main(int argc, char** argv) {
    // Configure the default severity Level of new Channel objects
#ifndef NDEBUG
    Log::Manager::setDefaultLevel(Log::Log::eDebug);
#else
    Log::Manager::setDefaultLevel(Log::Log::eNotice);
#endif

    // Configure the Output objects
    // TODO(SRombauts) Use a configuration file!
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

    log.info() << "freeglut starting...";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE     // double buffered window
                      | GLUT_ALPHA      // alpha component to the color buffer
                      | GLUT_DEPTH      // depth buffer
                      | GLUT_STENCIL    // stencil buffer (select which pixels to render)
                      | GLUT_SRGB);     // ask for gamma corrected rendering for sRGB colorspace
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(700, 0);

    log.notice() << "creating window...";
    int window = glutCreateWindow("OpenGL Experiments");

    log.debug() << "loading functions...";
    glload::LoadFunctions();
    log.notice() << "OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion();

    if (0 == glload::IsVersionGEQ(3, 3)) {
        log.error() << "You must have at least OpenGL 3.3";
        glutDestroyWindow(window);
    } else {
        // Create and initialize the application
        App app;

        // Main Loop (specifically asking to continue here on exiting main loop)
        log.notice() << "main loop starting...";
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
        glutMainLoop();
        log.notice() << "bye...";

        // Destructor of App will release memory
    }

    return 0;
}
