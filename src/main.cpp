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

#include <iostream>

#include <GL/freeglut.h>


/**
 * Simple display function - code from:
 *     http://fly.cc.fer.hr/~unreal/theredbook/chapter01.html
 * This is the actual usage of the OpenGL library.
 * The following code is the same for any platform
 */
void renderFunction()
{
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
 * @brief Main method - main entry point of application
 *
 *  freeglut does the window creation work for us regardless of the platform.
 */
int main(int argc, char** argv)
{
    std::cout << "freeglut starting..." << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);

    std::cout << "creating window..." << std::endl;
    /* int window = */ glutCreateWindow("OpenGL Experiments");
    
    glutDisplayFunc(renderFunction);
    glutMainLoop();

    return 0;
}
