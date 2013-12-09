/**
 * @file    Scene.h
 * @ingroup opengl-experiments
 * @brief   Root Nodes of a hierarchical scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Main/Node.h"

#include <glload/gl_3_3_comp.h>         // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...
#include <glm/gtc/quaternion.hpp>       // glm::fquat
#include <glutil/MatrixStack.h>

#include <vector>                       // std::vector


/**
 * @brief Root Nodes of a hierarchical scene graph
 */
class Scene {
public:
    Scene();
    ~Scene();

    // Draw
    inline void draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToWorldMatrixUnif) const;

    // Getters/Setters
    inline const Node::List&    getRootNodes() const;
    inline       void           addRootNode(const Node::Ptr& aRootNodePtr);

private:
    Node::List  mRootNodes; ///< Root Nodes of the current Scene

    // TODO(SRombauts) Add a
};


/**
 * @brief Draw the root nodes of the scene, and their children
 *
 * @param[in] aModelToCameraMatrixStack "Model to Camera" matrix stack
 * @param[in] aModelToCameraMatrixUnif  Location of the "modelToCameraMatrix" vertex shader uniform input variable
 */
inline void Scene::draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToCameraMatrixUnif) const {
    // Root of the stack : no transformation, no need to push the stack

    // Ask rote Nodes to draw themselves
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
