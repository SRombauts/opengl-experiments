/**
 * @file    Renderer.h
 * @ingroup Main
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

#include "Main/Scene.h"
#include "Main/Node.h"
#include "Utils/Utils.h"

#include <glload/gl_3_3_comp.h> // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...
#include <glutil/MatrixStack.h>

#include <assimp/scene.h>       // Assimp output data structure

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

    // Calculate new position and orientation given current Node movements
    inline void move(float aDeltaTime);

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
    void initScene();

    Node::Ptr loadFile(const char* apFilename);
    Node::Ptr loadNode(const aiScene* apScene, const aiNode* apNode);

    /// @todo Generalize like the Node class (but Camera is the inverse of Model)
    glm::mat4 transform();

private:
    Log::Logger mLog;                   ///< Logger object to output runtime information

    GLuint mProgram;                    ///< OpenGL program containing compiled and linked shaders
    GLuint mPositionAttrib;             ///< Location of the "position" vertex shader attribute (input stream)
    GLuint mColorAttrib;                ///< Location of the "diffuseColor" vertex shader attribute (input stream)
    GLuint mNormalAttrib;               ///< Location of the "normal" vertex shader attribute (input stream)
    GLuint mModelToCameraMatrixUnif;    ///< Location of the "modelToCameraMatrix" vertex shader uniform input variable
    GLuint mCameraToClipMatrixUnif;     ///< Location of the "cameraToClipMatrix" vertex shader uniform input variable
    GLuint mDirToLightUnif;             ///< Location of the "dirToLight" vertex shader uniform input variable
    GLuint mLightIntensityUnif;         ///< Location of the "lightIntensity" vertex shader uniform input variable
    GLuint mAmbientIntensityUnif;       ///< Location of the "ambientIntensity" vertex shader uniform input variable

    glm::fquat  mCameraOrientation;     ///< Axis and angle of rotation of the camera
    glm::vec3   mCameraTranslation;     ///< Vector of translation of the camera

    glm::vec4   mDirToLight;            ///< Vector of directional light orientation (oriented toward the light)
    glm::vec4   mLightIntensity;        ///< Directional light intensity and color
    glm::vec4   mAmbientIntensity;      ///< Ambiant light intensity and color

    Scene       mSceneHierarchy;        ///< Scene node hierarchy
    Node::Ptr   mModelPtr;              ///< The loadble/movable model
    Node::Ptr   mTurretPtr;             ///< The turret sub-model

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};


/**
 * @brief Calculate new position and orientation given current Node movements
 *
 * @param[in] aDeltaTime    Time elapsed since last movement (in seconds)
 */
inline void Renderer::move(float aDeltaTime) {
    mSceneHierarchy.move(aDeltaTime);
}
