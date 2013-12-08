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

#include <glload/gl_3_3_comp.h>         // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...
#include <glm/gtc/matrix_transform.hpp> // glm::rotate, glm::translate
#include <glm/gtc/quaternion.hpp>       // glm::fquat
#include <glutil/MatrixStack.h>

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

    // Draw
    void draw(glutil::MatrixStack& aModelToCameraMatrixStack, GLuint aModelToWorldMatrixUnif) const;

    // Calculate and return the current Rotations & Translations matrix
    const glm::mat4& getMatrix() const;

    inline const List&  getChildren() const;

private:
    Node::List          mChildrenList;          ///< Children Nodes of the current Node

    glm::fquat          mOrientationQuaternion; ///< Quaternion of orientation of the Node
    glm::vec3           mTranslationVector;     ///< Vector of translation of the Node

    // Mutable to enable updating in const getter
    mutable glm::mat4   mMatrix;                ///< Composed resulting Matrix of orientation and translation
    mutable bool        mbMatrixDirty;          ///< Tell if the composed Matrix is up to date or need recalculation
};

/**
 * @brief   Get the list of children of the current Node
 *
 * @return  Const Vector of children of the current Node
 */
inline const Node::List& Node::getChildren() const {
    return mChildrenList;
}
