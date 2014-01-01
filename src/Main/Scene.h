/**
 * @file    Scene.h
 * @ingroup Main
 * @brief   Container for root Nodes of a hierarchical Scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Main/Node.h"
#include "Main/MatrixStack.h"

#include <glload/gl_3_3_comp.h>         // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...
#include <glm/gtc/quaternion.hpp>       // glm::fquat

#include <vector>                       // std::vector


/**
 * @brief   Container for root Nodes of a hierarchical Scene graph
 * @ingroup Main
 *
 *  This base level of a scene graph does not have a matrix of transformation of its own.
 * It does not contain any mesh objects, and thus do no drawing at all.
 */
class Scene {
public:
    Scene();
    ~Scene();

    // Calculate new position and orientation given current Node movements
    inline void move(float aDeltaTime);

    // Draw
    inline void draw(MatrixStack& aModelToCameraMatrixStack, GLuint aModelToWorldMatrixUnif) const;

    // Getters/Setters
    inline const Node::List&    getRootNodes() const;
    inline       void           addRootNode(const Node::Ptr& aRootNodePtr);

private:
    Node::List  mRootNodes; ///< Root Nodes of the current Scene

    // TODO(SRombauts) Add Camera (or stereoscopic camera) object
    // TODO(SRombauts) Add Lights objects
};


/**
 * @brief Calculate new position and orientation given current Node movements
 *
 * @param[in] aDeltaTime    Time elapsed since last movement (in seconds)
 */
inline void Scene::move(float aDeltaTime) {
    // Ask root Nodes to move themselves
    for (Node::List::const_iterator iChild = mRootNodes.begin(); iChild != mRootNodes.end(); ++iChild) {
        (*iChild)->move(aDeltaTime);
    }
}

/**
 * @brief Draw the root nodes of the scene, and their children
 *
 * @param[in] aModelToCameraMatrixStack "Model to Camera" matrix stack
 * @param[in] aModelToCameraMatrixUnif  Location of the "modelToCameraMatrix" vertex shader uniform input variable
 */
inline void Scene::draw(MatrixStack& aModelToCameraMatrixStack, GLuint aModelToCameraMatrixUnif) const {
    // Root of the stack : no transformation, no need to push the stack

    // Ask root Nodes to draw themselves
    for (Node::List::const_iterator iChild = mRootNodes.begin(); iChild != mRootNodes.end(); ++iChild) {
        (*iChild)->draw(aModelToCameraMatrixStack, aModelToCameraMatrixUnif);
    }
}

/**
 * @brief   Get the list of children of the current Scene
 *
 * @return  Const Vector of children of the current Scene
 */
inline const Node::List& Scene::getRootNodes() const {
    return mRootNodes;
}

/**
 * @brief   Add a child Scene to the current Scene
 *
 * @param[in] aChildScenePtr Child Scene to add
 */
inline void Scene::addRootNode(const Node::Ptr& aChildScenePtr) {
    mRootNodes.push_back(aChildScenePtr);
}
