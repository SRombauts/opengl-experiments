/**
 * @file    ShaderProgram.cpp
 * @brief   Compile and link shaders into a program object
 * @ingroup Main
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/ShaderProgram.h"

#include "Utils/Exception.h"

#include <fstream>      // NOLINT(readability/streams) for shader files
#include <sstream>

#include <string>

/**
 * @brief Constructor
 */
ShaderProgram::ShaderProgram() :
    mLog("ShaderProgram") {}

/**
 * @brief Destructor
 */
ShaderProgram::~ShaderProgram() {
}

/**
 * @brief Compile a shader of the given type from the content of a file, and add it to the list
 *
 * @param[in] aShaderType       Type of shader to be compiled
 * @param[in] apShaderFilename  Name of the shader file to compile
 *
 * @throw a std::exception in case of error (std::runtime_error)
 */
void ShaderProgram::compileShader(const GLenum  aShaderType,
                                  const char*   apShaderFilename) {
    std::ifstream ifShader(apShaderFilename);
    if (ifShader.is_open()) {
        std::ostringstream isShader;
        isShader << ifShader.rdbuf();
        mLog.debug() << "compileShader: compiling \"" << apShaderFilename << "\"...";
        try {
            mShaderList.push_back(compileShader(aShaderType, isShader.str()));
        }
        catch(std::exception& e) {
            mLog.info() << "compileShader: \"" << apShaderFilename << "\":\n" << e.what();
            throw;  // rethrow to abort program
        }
    } else {
        mLog.critic() << "compileShader: unavailable file \"" << apShaderFilename << "\"";
        UTILS_THROW("compileShader: unavailable file " << apShaderFilename);
    }
}

/**
 * @brief Compile a shader of the given type from the source code provided as a string
 *
 * @param[in] aShaderType   Type of shader to be compiled
 * @param[in] aShaderSource Shader source code to compile
 *
 * @return Id of the created shader object
 *
 * @throw a std::exception in case of error (std::runtime_error)
 */
GLuint ShaderProgram::compileShader(GLenum              aShaderType,
                                    const std::string&  aShaderSource) const {
    const char* pShaderSource = aShaderSource.c_str();

    // Create a shader, give it the source code, and compile it
    GLuint shader = glCreateShader(aShaderType);
    glShaderSource(shader, 1, &pShaderSource, NULL);
    glCompileShader(shader);

    // Check shader status, and get error message if a problem occured
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (GL_FALSE == status) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* pInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, pInfoLog);
        std::string errorMsg = pInfoLog;
        delete[] pInfoLog;
        pInfoLog = NULL;

        UTILS_THROW(errorMsg);
    }

    return shader;
}

/**
 * @brief Link a list of shaders into a program object
 *
 * @return Id of the created program object
 *
 * @throw a std::exception in case of error (std::runtime_error)
 */
GLuint ShaderProgram::linkProgram() const {
    // Create a program, attach shaders to it, and link the program
    GLuint program = glCreateProgram();
    for (size_t idxShader = 0; idxShader < mShaderList.size(); ++idxShader) {
        glAttachShader(program, mShaderList[idxShader]);
    }
    glLinkProgram(program);

    // Check program status, and get error message if a problem occured
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* pInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, pInfoLog);
        std::string errorMsg = pInfoLog;
        delete[] pInfoLog;
        pInfoLog = NULL;

        UTILS_THROW(errorMsg);
    }

    // Now, the intermediate compiled shader can be detached and deleted (the program contain them)
    for (size_t idxShader = 0; idxShader < mShaderList.size(); ++idxShader) {
        glDetachShader(program, mShaderList[idxShader]);
        glDeleteShader(mShaderList[idxShader]);
    }

    return program;
}
