/**
 * @file    main.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <GL/freeglut.h>

#include <iostream>     // NOLINT(readability/streams) pour std::cout


/**
 * @brief GLUT reshape callback function
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void reshapeCallback(int aW, int aH) {
    std::cout << "reshapeCallback(" << aW << "," << aH << ")" << std::endl;

    glViewport(0, 0, (GLsizei)aW, (GLsizei)aH);
}

/**
 * @brief GLUT display callback function
 */
void displayCallback(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f);
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(-0.5f, 0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(0.5f, -0.5f);
    glEnd();
    glFlush();

    glutSwapBuffers();
    glutPostRedisplay();
}

/**
 * @brief GLUT keyboard callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */
void keyboardCallback(unsigned char aKey, int aX, int aY) {
    std::cout << "keyboardCallback(" << aKey << "," << aX << "," << aY << ")" << std::endl;
    switch (aKey) {
        case 27:  // ESC
            glutLeaveMainLoop();
            break;
        case 'w':
        case 'z':
            std::cout << "up" << std::endl;
            break;
        case 'a':
        case 'q':
            std::cout << "left" << std::endl;
            break;
        case 's':
            std::cout << "down" << std::endl;
            break;
        case 'd':
            std::cout << "right" << std::endl;
            break;
        default:
            break;
    }
}

/**
 * @brief GLUT mouse button click callback function
 *
 * @param[in] aButton   Mouse button
 * @param[in] aState    State of the button
 * @param[in] aX        X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY        Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void mouseCallback(int aButton, int aState, int aX, int aY) {
    std::cout << "mouseCallback(" << aButton << "," << aState << "," << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void motionCallback(int aX, int aY) {
    std::cout << "motionCallback(" << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief GLUT mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coordinate of the mouse cursor (0 is the left of the render surface of the window : can be negative !)
 * @param[in] aY    Y coordinate of the mouse cursor (0 is the top of the render surface of the window : can be negative !)
 */    
void passiveMotionCallback(int aX, int aY) {
    std::cout << "passiveMotionCallback(" << aX << "," << aY << ")" << std::endl;
}

/**
 * @brief Main method - main entry point of application
 *
 *  freeglut does the window creation work for us regardless of the platform.
 */
int main(int argc, char** argv) {
    std::cout << "freeglut starting..." << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);

    std::cout << "creating window..." << std::endl;
    int window = glutCreateWindow("OpenGL Experiments");

    std::cout << "loading functions..." << std::endl;
    glload::LoadFunctions();
    std::cout << "OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << std::endl;

    if (!glload::IsVersionGEQ(3, 3)) {
        std::cout << "You must have at least OpenGL 3.3" << std::endl;
        glutDestroyWindow(window);
        return 0;
    }

    std::cout << "set functions..." << std::endl;

    glutReshapeFunc(reshapeCallback);
    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotionCallback);

    std::cout << "main loop starting..." << std::endl;

    // Main Loop
    glutMainLoop();

    std::cout << "bye..." << std::endl;

    return 0;
}
