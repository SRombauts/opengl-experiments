/**
 * @file    Node.h
 * @ingroup opengl-experiments
 * @brief   Node of a scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Utils/shared_ptr.hpp"

#include <glload/gl_load.hpp>   // Need to be included before other gl library
#include <glload/gl_3_2_comp.h>
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...

#include <vector>               // std::vector


/**
 * @brief Node of a scene graph
 */
class Node {
public:
    typedef Utils::shared_ptr<Node> Ptr;    ///< Shared Pointer to a Node
    typedef std::vector<Ptr>        Vector; ///< Vector of pointers to a Node

public:
    Node();
    ~Node();

    inline const Vector& getChildren() const;
    inline       Vector& getChildren();

private:
    Node::Vector    mChildren;          ///< Children Node of the current Node

    glm::vec3       mModelTranslation;  ///< Vector of translation of the model
};

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Const Vector of children of the current Node
 */
inline const Node::Vector& Node::getChildren() const {
    return mChildren;
}

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Vector of children of the current Node
 */
inline Node::Vector& Node::getChildren() {
    return mChildren;
}
