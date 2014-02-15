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

#include <memory>           // std::shared_ptr

// NOTE: Needs to be included before any other gl/glfw/freeglut header
#include <glload/gl_3_3.h>  // GLuint, GLenum, and OpenGL 3.3 core function APIs
#include <glm/glm.hpp>      // glm::mat4, glm::vec3... (GLM_FORCE_RADIANS defined at the project level)

#include <vector>           // std::vector
#include <string>           // std::string

/**
 * @brief Description of a mesh/model at a Node of the Scene
 * @ingroup Main
 */
class Mesh {
public:
    // TODO unique_ptr ?
    typedef std::shared_ptr<Mesh>   Ptr;        ///< Shared Pointer to a Mesh
    typedef std::vector<Ptr>        List;       ///< List (std::vector) of pointers of Mesh

    typedef std::vector<glm::vec3>  VertexData; ///< A Vector of Vertex data composed of 3 float elements
    typedef std::vector<GLshort>    IndexData;  ///< A Vector of Index data composed of short elements

public:
    Mesh(const char* apName,
         GLenum aPrimitiveType,
         GLuint aElementCount,
         GLenum aIndexDataType,
         GLuint aStartPosition);
    ~Mesh();

    // Generate OpenGL objects
    void genOpenGlObjects(const VertexData& aVertexData,
                          const IndexData&  aIndexData,
                          GLuint            aPositionAttrib,
                          GLuint            aColorAttrib,
                          GLuint            aNormalAttrib);
    void deleteOpenGlObjects();

    /**
     * @brief Indexed Draw Call glDrawElements()
     */
    inline void draw() const {
        mDrawCall.draw(mVertexArrayObject);
    }

    // Getter
    inline const std::string& getName() const;

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
    const std::string   mName;  ///< Name of the Node

    GLuint mVertexBufferObject; ///< VBO: Vertex Buffer Object containing the data of our Mesh (vertices, normals, UVs)
    GLuint mIndexBufferObject;  ///< IBO: Index Buffer Object containing the indices of vertices of our Mesh
    GLuint mVertexArrayObject;  ///< VAO: Vertex Array Object retaining the states needed for the render calls

    IndexedDrawCall mDrawCall;  ///< Indexed draw call of the current Mesh
};

/**
 * @brief   Get the Name of the current Node
 *
 * @return  Const string whit the name of the current Node
 */
inline const std::string& Mesh::getName() const {
    return mName;
}
