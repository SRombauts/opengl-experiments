/**
 * @file    Node.cpp
 * @ingroup opengl-experiments
 * @brief   Node of a scene graph
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Node.h"


/**
 * @brief Constructor
 */
Node::Node() :
    mbMatrixDirty(false) {
}

/**
 * @brief Destructor
 */
Node::~Node() {
}

