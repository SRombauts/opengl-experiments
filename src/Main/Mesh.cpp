/**
 * @file    Mesh.cpp
 * @ingroup Main
 * @brief   Description of a mesh/model at a #Node of the #Scene
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Mesh.h"


/**
 * @brief Constructor
 *
 * @param[in] apName            Name of the new Node
 * @param[in] aPrimitiveType    GL_TRIANGLES, GL_TRIANGLE_STRIP...
 * @param[in] aElementCount     Number of indexed vertex to draw
 * @param[in] aIndexDataType    GL_UNSIGNED_SHORT...
 * @param[in] aStartPosition    Offset in bytes from where start indices in the buffer
 */
Mesh::Mesh(const char* apName,
           GLenum aPrimitiveType,
           GLuint aElementCount,
           GLenum aIndexDataType,
           GLuint aStartPosition) :
    mName(apName),
    mVertexBufferObject(0),
    mIndexBufferObject(0),
    mVertexArrayObject(0),
    mDrawCall(aPrimitiveType,
              aElementCount,
              aIndexDataType,
              aStartPosition) {
}

/**
 * @brief Initialize the Vertex Buffer, Index Buffer and Vertex Array Objects
 *
 *  Init the VBO (Vertex Buffer Object) with the data of our mesh (vertex positions, colors, and normals),
 * same for the IBO (Index Buffer Object) with short integers pointing to vertex data (forming triangle list),
 * and retain all the states needed with a VAO (Vertex Array Object)
 *
 * @param[in] aVertexData       Vertex data (vertex positions, colors, and normals)
 * @param[in] aIndexData        Index data (triangle list)
 * @param[in] aPositionAttrib   Location of the "position" vertex shader attribute (input stream)
 * @param[in] aColorAttrib      Location of the "diffuseColor" vertex shader attribute (input stream)
 * @param[in] aNormalAttrib     Location of the "normal" vertex shader attribute (input stream)
 */
void Mesh::genOpenGlObjects(const VertexData&   aVertexData,
                            const IndexData&    aIndexData,
                            GLuint              aPositionAttrib,
                            GLuint              aColorAttrib,
                            GLuint              aNormalAttrib) {
    // Generate a VBO: Ask for a buffer of GPU memory
    glGenBuffers(1, &mVertexBufferObject);
    assert(0 != mVertexBufferObject); /// @todo test buffers != 0 with a dedicated ASSERT_VBO

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    /// @todo use templates to get size and buffer
    glBufferData(GL_ARRAY_BUFFER, aVertexData.size() * sizeof(aVertexData[0]), &aVertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // here aVertexData is of no more use (dynamic memory will be deallocated)

    // Generate a IBO: Ask for a buffer of GPU memory
    glGenBuffers(1, &mIndexBufferObject);

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    /// @todo use templates to get size and start of buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, aIndexData.size() * sizeof(aIndexData[0]), &aIndexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // here _indexData is of no more use (dynamic memory could be deallocated)

    // Generate a VAO: Ask for a place on GPU to associate states with our data
    glGenVertexArrays(1, &mVertexArrayObject);

    // Bind the vertex array, so that it can memorize the following states
    glBindVertexArray(mVertexArrayObject);

    // Bind the vertex buffer, and init vertex position and colors input streams (shader attributes)
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glEnableVertexAttribArray(aPositionAttrib); // layout(location = 0) in vec4 position;
    glEnableVertexAttribArray(aColorAttrib);    // layout(location = 1) in vec4 diffuseColor;
    glEnableVertexAttribArray(aNormalAttrib);   // layout(location = 2) in vec3 normal;

    // this tells the GPU witch part of the buffer to route to which attribute (shader input stream)
    const size_t vertexDim = 3;
    glVertexAttribPointer(aPositionAttrib,  vertexDim, GL_FLOAT, GL_FALSE, 3 * sizeof(aVertexData[0]),
            reinterpret_cast<void*>(0));
    glVertexAttribPointer(aColorAttrib,     vertexDim, GL_FLOAT, GL_FALSE, 3 * sizeof(aVertexData[0]),
            reinterpret_cast<void*>(sizeof(aVertexData[0])));
    glVertexAttribPointer(aNormalAttrib,    vertexDim, GL_FLOAT, GL_FALSE, 3 * sizeof(aVertexData[0]),
            reinterpret_cast<void*>(2*sizeof(aVertexData[0])));
    // this tells OpenGL that vertex are pointed by index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

    glBindVertexArray(0);
}

/**
 * @brief Indexed Draw Call glDrawElements()
 *
 * @param[in] aVertexArrayObject    VAO: Vertex Array Object retaining the states needed for the render calls
 */
void Mesh::IndexedDrawCall::draw(GLuint aVertexArrayObject) const {
    // Bind the Vertex Array Object, bound to buffers with vertex position and colors
    glBindVertexArray(aVertexArrayObject);

    // Emit the OpenGL draw call
    glDrawElements(mPrimitiveType, mElementCount, mIndexDataType, reinterpret_cast<void*>(mStartPosition));

    // Unbind the Vertex Array Object
    glBindVertexArray(0);
}


/**
 * @brief Uninitialize the vertex buffer and vertex array objects
 */
void Mesh::deleteOpenGlObjects(void) {
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

/**
 * @brief Destructor
 */
Mesh::~Mesh() {
    deleteOpenGlObjects();
}

