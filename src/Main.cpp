/**
 * @file    Main.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>
#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>      // NOLINT(readability/streams) for files
#include <sstream>
#include <string>
#include <vector>
#include <cassert>


#include "App.h"


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
    Log::Config::Vector configList;
    Log::Config::addOutput(configList, "OutputConsole");
    Log::Config::addOutput(configList, "OutputFile");
    Log::Config::setOption(configList, "filename",          "log.txt");
    Log::Config::setOption(configList, "filename_old",      "log.old.txt");
    Log::Config::setOption(configList, "max_startup_size",  "0");
    Log::Config::setOption(configList, "max_size",          "10000");
#ifdef WIN32
    Log::Config::addOutput(configList, "OutputDebug");
#endif
    Log::Manager::configure(configList);
    Log::Logger log("main");

    log.info() << "freeglut starting...";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);

    log.notice() << "creating window...";
    int window = glutCreateWindow("OpenGL Experiments");

    log.debug() << "loading functions...";
    glload::LoadFunctions();
    log.notice() << "OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion();

    if (0 == glload::IsVersionGEQ(3, 3)) {
        log.error() << "You must have at least OpenGL 3.3";
        glutDestroyWindow(window);
        return 0;
    }

    App app;

    // 1) compile shaders and link them in a program
    app.initProgram();
    
    // 2) init the vertex buffer
    app.initVertexBufferObject();

    // 3) Register GLUT Callbacks
    app.registerCallbacks();

    // Main Loop
    log.notice() << "main loop starting...";
    glutMainLoop();
    log.notice() << "bye...";

    return 0;
}
