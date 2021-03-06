/**
 * @file    Renderer.h
 * @ingroup Main
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

#include "Main/Scene.h"
#include "Main/Node.h"
#include "Utils/Utils.h"

// NOTE: Needs to be included before any other gl/glfw/freeglut header
#include <glload/gl_3_3.h>      // GLuint, GLenum, and OpenGL 3.3 core function APIs
#include <glm/glm.hpp>          // glm::mat4, glm::vec3... (GLM_FORCE_RADIANS defined at the project level)

#include <assimp/scene.h>       // Assimp output data structure

#include <vector>

namespace Utils {
    class FPS;
}

/**
 * @brief Management of OpenGL drawing/rendering
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    // called by gflw through Input
    void reshape(int aW, int aH);
    void display();

    // Calculate new position and orientation given current Node movements
    inline void move(float aDeltaTime);

    // Increment/decrement the screen center offset
    inline void incrScreenCenterOffset(float aOffset);

    // called by Input::checkKeys()
    // camera:
    void move(const glm::vec3& aTranslation);
    void setCameraOrientation(const glm::fquat& aCameraOrientation);
    // model:
    void modelMove(const glm::vec3& aTranslation);
    void modelPitch(float aAngle);
    void modelYaw(float aAngle);
    void modelRoll(float aAngle);

private:
    // Initialization
    void init();
    void initProgram();
    void initScene();

    Node::Ptr loadFile(const char* apFilename);
    Node::Ptr loadNode(const aiScene* apScene, const aiNode* apNode);

    /// @todo Generalize like the Node class (but Camera is the inverse of Model)
    glm::mat4 getWorldToCameraMatrix(int aIdxEye);

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

    glm::fquat  mCameraOrientation;     ///< Quaternion of camera orientation
    glm::vec3   mCameraTranslation;     ///< Vector of translation of the camera

    glm::vec4   mDirToLight;            ///< Vector of directional light orientation (oriented toward the light)
    glm::vec4   mLightIntensity;        ///< Directional light intensity and color
    glm::vec4   mAmbientIntensity;      ///< Ambiant light intensity and color

    Scene       mSceneHierarchy;        ///< Scene node hierarchy
    Node::Ptr   mModelPtr;              ///< The loadble/movable model
    Node::Ptr   mTurretPtr;             ///< The turret sub-model

    int         mScreenWidth;           ///< Screen width
    int         mScreenHeight;          ///< Screen height
    float       mScreenCenterOffset;    ///< Screen center offset for each eye, in meters

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

/**
 * @brief Increment/decrement the screen center offset
 *
 * @param[in] aOffset Horizontal offset (in meters) to add/remove
 */ 
inline void Renderer::incrScreenCenterOffset(float aOffset) {
    mScreenCenterOffset += aOffset;
    mLog.info() << "SetScreenCenterOffset(" << mScreenCenterOffset << "m)";
}
