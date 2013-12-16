/**
 * @file    Mesh.h
 * @ingroup Main
 * @brief   Description of a mesh/model at a #Node of the #Scene
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


/**
 * @brief Description of a mesh/model at a #Node of the #Scene
 */
class Mesh {
public:
    Mesh();
    ~Mesh();

    // TODO(SRombauts) Move here the DrawCall stuff from the Node class

private:
    GLuint mVertexBufferObject; ///< VBO: Vertex Buffer Object containing the data of our Mesh (vertices, normals, UVs)
    GLuint mIndexBufferObject;  ///< Index buffer object containing the indices of vertices of our Mesh
    GLuint mVertexArrayObject;  ///< VAO: Vertex Array Object retaining the states needed for the render calls
};
