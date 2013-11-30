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

#include "Utils/shared_ptr.hpp"         // std::shared_ptr replacement

#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...
#include <glm/gtc/matrix_transform.hpp> // glm::rotate, glm::translate
#include <glm/gtc/quaternion.hpp>       // glm::fquat

#include <vector>                       // std::vector


/**
 * @brief Node of a scene graph
 */
class Node {
public:
    typedef Utils::shared_ptr<Node> Ptr;    ///< Shared Pointer to a Node
    typedef std::vector<Ptr>        List;   ///< List (std::vector) of pointers to a Node

public:
    Node();
    ~Node();

    inline const List&      getChildren() const;
    inline       List&      getChildren();

    inline const glm::mat4& getMatrix() const;

private:
    Node::List          mChildrenList;          ///< Children Nodes of the current Node

    glm::fquat          mOrientationQuaternion; ///< Quaternion of orientation of the Node
    glm::vec3           mTranslationVector;     ///< Vector of translation of the Node

    // Mutable to enable updating in const getter
    mutable glm::mat4   mMatrix;                ///< Composited resulting Matrix of orientation and translation
    mutable bool        mbMatrixDirty;          ///< Tell if the composited Matrix is up to date or need recalculation
};

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Const Vector of children of the current Node
 */
inline const Node::List& Node::getChildren() const {
    return mChildrenList;
}

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Vector of children of the current Node
 */
inline Node::List& Node::getChildren() {
    return mChildrenList;
}

/**
 * @brief   Get the up-to-date composited Matrix of orientation and translation
 *
 *  Recalculate the matrix from quaternion of orientation and vector of translation only when required.
 * Resulting matrix is cached until a change in orientation or translation flag it as "dirty".
 *
 * @return  Vector of children of the current Node
 */
inline const glm::mat4& Node::getMatrix() const {
    if (mbMatrixDirty) {
        glm::mat4 rotation  = glm::mat4_cast(mOrientationQuaternion);
        mMatrix             = glm::translate(rotation, mTranslationVector);
    }

    return mMatrix;
}
