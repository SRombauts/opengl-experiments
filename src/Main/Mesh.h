/**
 * @file    Mesh.h
 * @ingroup Main
 * @brief   Description of a mesh/model at a Node of the Scene
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/shared_ptr.hpp"         // std::shared_ptr replacement

#include <glload/gl_3_3_comp.h>         // GLuint, GLenum
#define GLM_FORCE_RADIANS // Using radians
#include <glm/glm.hpp>                  // glm::mat4, glm::vec3...

#include <vector>                       // std::vector


/**
 * @brief Description of a mesh/model at a Node of the Scene
 * @ingroup Main
 */
class Mesh {
public:
    typedef Utils::shared_ptr<Mesh> Ptr;        ///< Shared Pointer to a Mesh
    typedef std::vector<Ptr>        List;       ///< List (std::vector) of pointers of Mesh

    typedef std::vector<glm::vec3>  VertexData; ///< A Vector of Vertex data composed of 3 float elements
    typedef std::vector<GLshort>    IndexData;  ///< A Vector of Index data composed of short elements

public:
    Mesh(GLenum aPrimitiveType,
         GLuint aElementCount,
         GLenum aIndexDataType,
         GLuint aStartPosition);
    ~Mesh();

    void genOpenGlObjects(const VertexData& aVertexData,
                               const IndexData& aIndexData,
                               GLuint aPositionAttrib,
                               GLuint aColorAttrib,
                               GLuint aNormalAttrib);
    void deleteOpenGlObjects();

    /**
     * @brief Indexed Draw Call glDrawElements()
     */
    inline void draw() const {
        mDrawCall.draw(mVertexArrayObject);
    }

private:
    /**
     * @brief Public structure managing an OpenGL indexed draw call
     */
    class IndexedDrawCall {
     public:
        /**
         * @brief Constructor
         *
         * @param[in] aPrimitiveType    GL_TRIANGLES, GL_TRIANGLE_STRIP...
         * @param[in] aElementCount;    Number of indexed vertex to draw
         * @param[in] aIndexDataType;   GL_UNSIGNED_SHORT...
         * @param[in] aStartPosition;   Offset in bytes from where start indices in the buffer
         */
        inline IndexedDrawCall(GLenum aPrimitiveType,
                               GLuint aElementCount,
                               GLenum aIndexDataType,
                               GLuint aStartPosition) :
            mPrimitiveType(aPrimitiveType),
            mElementCount(aElementCount),
            mIndexDataType(aIndexDataType),
            mStartPosition(aStartPosition) {
        }

        void draw(GLuint aVertexArrayObject) const;

     private:
        GLenum mPrimitiveType;  ///< GL_TRIANGLES, GL_TRIANGLE_STRIP...
        GLuint mElementCount;   ///< Number of indexed vertex to draw
        GLenum mIndexDataType;  ///< GL_UNSIGNED_SHORT...
        GLuint mStartPosition;  ///< Offset in bytes from where start indices in the buffer
    };

private:
    GLuint mVertexBufferObject; ///< VBO: Vertex Buffer Object containing the data of our Mesh (vertices, normals, UVs)
    GLuint mIndexBufferObject;  ///< IBO: Index Buffer Object containing the indices of vertices of our Mesh
    GLuint mVertexArrayObject;  ///< VAO: Vertex Array Object retaining the states needed for the render calls

    IndexedDrawCall mDrawCall;  ///< Indexed draw call of the current Mesh
};
