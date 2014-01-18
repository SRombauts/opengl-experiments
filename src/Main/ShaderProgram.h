/**
 * @file    ShaderProgram.h
 * @ingroup Main
 * @brief   Compile and link shaders into a program object
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

#include "Utils/Utils.h"

// NOTE: Needs to be included before any other gl/glfw/freeglut header
#include <glload/gl_3_3.h>  // GLuint, GLenum, and OpenGL 3.3 core function APIs

#include <vector>
#include <string>


/**
 * @brief   Compile and link shaders into a program object
 * @ingroup Main
 */
class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    void    compileShader(const GLenum aShaderType, const char* apShaderFilename);
    GLuint  linkProgram() const;

private:
    GLuint  compileShader(const GLenum aShaderType, const std::string& aShaderSource) const;

private:
    typedef std::vector<GLuint> ShaderList; ///< List of OpenGL compiled shaders

private:
    Log::Logger mLog;           ///< Logger object to output runtime information

    ShaderList  mShaderList;    ///< List of OpenGL compiled shaders

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
};
