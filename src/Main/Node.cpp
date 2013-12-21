/**
 * @file    Node.cpp
 * @ingroup Main
 * @brief   Node of a scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Node.h"

#include <glm/gtc/matrix_transform.hpp> // glm::perspective, glm::rotate, glm::translate


// We use a standard "Right Hand Coordinate System"
// UNIT_X_RIGHT, UNIT_Y_UP and UNIT_Z_FRONT are the unit vectors of the world coordinate system
const glm::vec3 Node::UNIT_X_RIGHT(1.0f, 0.0f, 0.0f);   // "right of the world"
const glm::vec3 Node::UNIT_Y_UP(0.0f, 1.0f, 0.0f);      // "up of the world"
const glm::vec3 Node::UNIT_Z_FRONT(0.0f, 0.0f, 1.0f);   // "front of the world"


/**
 * @brief Constructor
 *
 * @param[in] apName    Name of the new Node
 */
Node::Node(const char* apName) :
    mName(apName),
    mbMatrixDirty(false) {
}

/**
 * @brief Destructor
 */
Node::~Node() {
}


/**
 * @brief Move the move from the given relative translation vector
 *
 * @param[in] aTranslation  3D Translation vector to add to the given camera position
 */
void Node::move(const glm::vec3& aTranslation) {
    // Get the rotation matrix from the orientation quaternion:
    glm::mat3 rotations = glm::mat3_cast(mOrientationQuat);
    // calculate relative translation into the current model orientation
    const glm::vec3 relativeTranslation = (rotations * aTranslation);
    // and apply it to the current model position
    mTranslationVector += relativeTranslation;

    mbMatrixDirty = true;
}

/**
 * @brief Pitch, rotate the model vertically around its current relative horizontal X axis
 */
void Node::pitch(float aAngle) {
    // calculate X unit vector of the current camera orientation
    const glm::vec3 modelX = (mOrientationQuat * UNIT_X_RIGHT);
    // Offset the given quaternion by the given angle (in radians) and normalized axis
    rotateLeftMultiply(mOrientationQuat, aAngle, modelX);

    mbMatrixDirty = true;
}

/**
 * @brief Yaw, rotate the model horizontally around its current relative vertical Y axis
 */
void Node::yaw(float aAngle) {
    // calculate Y unit vector of the current camera orientation
    const glm::vec3 modelY = (mOrientationQuat * UNIT_Y_UP);
    // Offset the given quaternion by the given angle (in radians) and normalized axis
    rotateLeftMultiply(mOrientationQuat, aAngle, modelY);

    mbMatrixDirty = true;
}

/**
 * @brief Roll, rotate the model around its current relative viewing Z axis
 */
void Node::roll(float aAngle) {
    // calculate Z unit vector of the current camera orientation
    const glm::vec3 modelZ = (mOrientationQuat * UNIT_Z_FRONT);
    // Offset the given quaternion by the given angle (in radians) and normalized axis
    rotateLeftMultiply(mOrientationQuat, aAngle, modelZ);

    mbMatrixDirty = true;
}

/**
 * @brief   Get the up-to-date Matrix composed of relative orientation and translation
 *
 *  Recalculate the matrix from quaternion of orientation and vector of translation only when required.
 * Resulting matrix is cached until a change in orientation or translation flag it as "dirty".
 *
 *  We want to apply rotation first, then translation, but matrix have to be multiplied in reverse order :
 * out = (translation * rotation) * in;
 *
 * @return  Vector of children of the current Node
 */
inline const glm::mat4& Node::getMatrix() const {
    if (mbMatrixDirty) {
        // Translation matrix
        glm::mat4 translation   = glm::translate(glm::mat4(1.0f), mTranslationVector);
        // Rotation matrix
        glm::mat4 rotation      = glm::mat4_cast(mOrientationQuat);
        // Calculate the new relative "modelToWorldMatrix" (from right to left: rotation , then translation )
        mMatrix                 = (translation  * rotation );
    }

    return mMatrix;
}

/**
 * @brief Draw the node and its children
 *
 * @param[in] aModelToCameraMatrixStack "Model to Camera" matrix stack
 * @param[in] aModelToCameraMatrixUnif  Location of the "modelToCameraMatrix" vertex shader uniform input variable
 */
void Node::draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToCameraMatrixUnif) const {
    glutil::PushStack push(aModelToCameraMatrixStack); // RAII PushStack

    // Re-calculate the relative Model to World transformations matrix, and apply it to the stack
    // => this effectively build the absolute "modelToCameraMatrix"
    aModelToCameraMatrixStack.ApplyMatrix(getMatrix());

    // Set uniform values with this new "modelToCameraMatrix" matrix
    glUniformMatrix4fv(aModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(aModelToCameraMatrixStack.Top()));

    // Draw meshes of the current node
    for (Mesh::List::const_iterator iMesh = mMeshesList.begin(); iMesh != mMeshesList.end(); ++iMesh) {
        (*iMesh)->draw();
    }

    // And ask children to draw themselves
    for (List::const_iterator iChild = mChildrenList.begin(); iChild != mChildrenList.end(); ++iChild) {
        (*iChild)->draw(aModelToCameraMatrixStack, aModelToCameraMatrixUnif);
    }
}

/**
 * @brief Rotate the given quaternion by the given angle and axis, relatively to the world (by right-multiplying)
 * 
 * @param[in,out]   aOrientation    Quaternion to offset
 * @param[in]       aAngleRad       Angle of the rotation in radians
 * @param[in]       aAxis           Vector axis of the rotation (must be normalized)
*/ 
void Node::rotateRightMultiply(glm::fquat& aOrientation, float aAngleRad, const glm::vec3 &aAxis) {
    // build an offset quaternion from angle and axis
    const glm::fquat offset = glm::angleAxis(aAngleRad, aAxis);
    // right multiply the orientation with the offset quaternion
    aOrientation = glm::normalize(aOrientation * offset);
}

/**
 * @brief Rotate the given quaternion by the given angle and axis, relatively to the model (by left-multiplying)
 * 
 * @param[in,out]   aOrientation    Quaternion to offset
 * @param[in]       aAngleRad       Angle of the rotation in radians
 * @param[in]       aAxis           Vector axis of the rotation (must be normalized)
*/ 
void Node::rotateLeftMultiply(glm::fquat& aOrientation, float aAngleRad, const glm::vec3 &aAxis) {
    // build an offset quaternion from angle and axis
    const glm::fquat offset = glm::angleAxis(aAngleRad, aAxis);
    // left multiply the orientation with the offset quaternion
    aOrientation = glm::normalize(offset * aOrientation);
}
