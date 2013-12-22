/**
 * @file    Node.h
 * @ingroup Main
 * @brief   Node of a Scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Main/Mesh.h"

#include "Utils/shared_ptr.hpp"         // std::shared_ptr replacement
#include "Utils/Utils.h"

#include <glload/gl_3_3_comp.h>         // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...
#include <glm/gtc/quaternion.hpp>       // glm::fquat
#include <glutil/MatrixStack.h>

#include <vector>                       // std::vector
#include <string>                       // std::string

/**
 * @brief Node of a Scene graph
 * @ingroup Main
 */
class Node {
public:
    // Base vector of a standard "Right Hand Coordinate System"
    static const glm::vec3 UNIT_X_RIGHT;    ///< Unit vector to the "right of the world"
    static const glm::vec3 UNIT_Y_UP;       ///< Unit vector to the "up of the world"
    static const glm::vec3 UNIT_Z_FRONT;    ///< Unit vector to the "front of the world"

public:
    typedef Utils::shared_ptr<Node> Ptr;        ///< Shared Pointer to a Node
    typedef std::vector<Ptr>        List;       ///< List (std::vector) of pointers to a Node

public:
    explicit Node(const char* apName);
    ~Node();

    // Basic movements
    void move(const glm::vec3& aTranslation);
    void pitch(float aAngle);
    void yaw(float aAngle);
    void roll(float aAngle);

    // Explicit setters (used at load time with Assimp)
    inline void setOrientationQuaternion(float w, float x, float y, float z);
    inline void setTranslationVector(float x, float y, float z);

    // Calculate and return the current Rotations & Translations matrix
    const glm::mat4& getMatrix() const;

    // Draw
    void draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToWorldMatrixUnif) const;

    // Getters/Setters
    inline const std::string& getName() const;
    inline const List&  getChildren() const;
    inline       void   addChildNode(const Node::Ptr& aChildNodePtr);
    inline       void   addMesh(const Mesh::Ptr& aMeshPtr);

    // Rotate a given quaternion by an axis and an angle
    static void rotateRightMultiply(glm::fquat& aCameraOrientation, float aAngRad, const glm::vec3 &aAxis);
    static void rotateLeftMultiply(glm::fquat& aCameraOrientation, float aAngRad, const glm::vec3 &aAxis);

private:
    const std::string   mName;                  ///< Name of the Node

    Node::List          mChildrenList;          ///< Children Nodes of the current Node
    Mesh::List          mMeshesList;            ///< List of Mesh(es) for the current Node

    glm::fquat          mOrientationQuaternion; ///< Quaternion of orientation of the Node
    glm::vec3           mTranslationVector;     ///< Vector of translation of the Node

    // Mutable to enable updating in const getter
    mutable glm::mat4   mMatrix;                ///< Composed resulting Matrix of orientation and translation
    mutable bool        mbMatrixDirty;          ///< Tell if the composed Matrix is up to date or need recalculation

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Node);
};

/**
 * @brief   Set the relative orientation of the Node (from its parent)
 *
 * @param[in]   w component of the quaternion of relative orientation of the Node
 * @param[in]   x component of the quaternion of relative orientation of the Node
 * @param[in]   y component of the quaternion of relative orientation of the Node
 * @param[in]   z component of the quaternion of relative orientation of the Node
 */
inline void Node::setOrientationQuaternion(float w, float x, float y, float z) {
    mOrientationQuaternion.w = w;
    mOrientationQuaternion.x = x;
    mOrientationQuaternion.y = y;
    mOrientationQuaternion.z = z;
}

/**
 * @brief   Set the relative translation of the Node (from its parent)
 *
 * @param[in]   x component of the quaternion of relative orientation of the Node
 * @param[in]   y component of the quaternion of relative orientation of the Node
 * @param[in]   z component of the quaternion of relative orientation of the Node
 */
inline void Node::setTranslationVector(float x, float y, float z) {
    mTranslationVector.x = x;
    mTranslationVector.y = y;
    mTranslationVector.z = z;
}

/**
 * @brief   Get the Name of the current Node
 *
 * @return  Const string whit the name of the current Node
 */
inline const std::string& Node::getName() const {
    return mName;
}

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Const Vector of children of the current Node
 */
inline const Node::List& Node::getChildren() const {
    return mChildrenList;
}

/**
 * @brief   Add a child Node to the current Node
 *
 * @param[in] aChildNodePtr Child Node to add 
 */
inline void Node::addChildNode(const Node::Ptr& aChildNodePtr) {
    mChildrenList.push_back(aChildNodePtr);
}

/**
 * @brief   Add a draw call to the current Node
 *
 * @param[in] aMeshPtr Draw call to add
 */
inline void Node::addMesh(const Mesh::Ptr& aMeshPtr) {
    mMeshesList.push_back(aMeshPtr);
}
