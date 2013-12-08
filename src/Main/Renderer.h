/**
 * @file    Renderer.h
 * @ingroup opengl-experiments
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

#include "Main/Node.h"
#include "Utils/Utils.h"

#include <glload/gl_3_3_comp.h> // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...
#include <glutil/MatrixStack.h>

#include <vector>


/**
 * @brief Management of OpenGL drawing/rendering
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    // called by Input freeglut callbacks
    void reshape(int aW, int aH);
    void display();

    // called by Input::checkKeys()
    // camera:
    void move(const glm::vec3& aTranslation);
    void pitch(float aAngle);
    void yaw(float aAngle);
    void roll(float aAngles);
    // model:
    void modelMove(const glm::vec3& aTranslation);
    void modelPitch(float aAngle);
    void modelYaw(float aAngle);
    void modelRoll(float aAngle);

private:
    // Initialization
    void init();
    void compileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const;
    void initProgram();
    void initVertexArrayObject();
    void initScene();
    void uninitVertexArrayObject();

    // TODO(SRombauts) Generalize with the Node class (Presently Camera is the inverse of Model)
    glm::mat4 transform();
    glm::mat4 modelTransform();

    // TODO(SRombauts) : remove this old stack
    void drawPlane(glutil::MatrixStack& aModelToCameraMatrixStack);
    void drawCube(glutil::MatrixStack& aModelToCameraMatrixStack);

private:
    Log::Logger mLog;                   ///< Logger object to output runtime information

    GLuint mProgram;                    ///< OpenGL program containing compiled and linked shaders
    GLuint mPositionAttrib;             ///< Location of the "position" vertex shader attribute (input stream)
    GLuint mColorAttrib;                ///< Location of the "color" vertex shader attribute (input stream)
    GLuint mModelToCameraMatrixUnif;    ///< Location of the "modelToCameraMatrix" vertex shader uniform input variable
    GLuint mCameraToClipMatrixUnif;     ///< Location of the "cameraToClipMatrix"  vertex shader uniform input variable

    GLuint mVertexBufferObject;         ///< Vertex buffer object containing the data of our mesh
    GLuint mIndexBufferObject;          ///< Index buffer object containing the indices of vertices of our mesh
    GLuint mVertexArrayObject;          ///< Vertex array object

    glm::fquat  mCameraOrientation;     ///< Axis and angle of rotation of the camera
    glm::vec3   mCameraTranslation;     ///< Vector of translation of the camera

    glm::fquat  mModelOrientation;      ///< Axis and angle of rotation of the model
    glm::vec3   mModelTranslation;      ///< Vector of translation of the model

    Node        mSceneHierarchy;        ///< Scene node hierarchy

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};

