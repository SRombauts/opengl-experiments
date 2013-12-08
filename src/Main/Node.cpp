/**
 * @file    Node.cpp
 * @ingroup opengl-experiments
 * @brief   Node of a scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Node.h"


/**
 * @brief Constructor
 */
Node::Node() :
    mbMatrixDirty(false) {
}

/**
 * @brief Destructor
 */
Node::~Node() {
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
        glm::mat4 rotation      = glm::mat4_cast(mOrientationQuaternion);
        // Calculate the new relative "modelToWorldMatrix" (from right to left: rotation , then translation )
        mMatrix                 = (translation  * rotation );
    }

    return mMatrix;
}

/**
 * @brief Draw the node and its children
 */
void Node::draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToWorldMatrixUnif) const {
    glutil::PushStack push(aModelToCameraMatrixStack); // RAII PushStack

    // re-calculate the absolute Model to World transformations matrix
    aModelToCameraMatrixStack.ApplyMatrix(getMatrix());

    // Set uniform values with the new "modelToWorldMatrix" matrix
    glUniformMatrix4fv(aModelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(aModelToCameraMatrixStack.Top()));

    // And ask children to draw themselves:
    for (List::const_iterator iChild = mChildrenList.begin(); iChild != mChildrenList.end(); ++iChild) {
        (*iChild)->draw(aModelToCameraMatrixStack, aModelToWorldMatrixUnif);
    }
}
