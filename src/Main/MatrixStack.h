/**
 * @file    MatrixStack.h
 * @ingroup Main
 * @brief   Wrapper of a std::stack of glm::mat4 matrices with a RAII PushStack interface.
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/Utils.h"

#include <glm/glm.hpp>

#include <stack>

/**
 * @brief Wrapper of a std::stack of glm::mat4 matrices with a RAII Push/Pop interface.
 * @ingroup Main
 *
 *  This is a header-only implementation of the classic concept of a stack of matrix,
 * used to store transformations of a Scene hierarchy in 3D engine.
 *  It maintains a "current matrix" that can be modified as needed.
 * When going down the Scene hierarchy, current matrix can be "push" into the stack.
 * When going back up, previously saved matrices can be restored/"poped" from the stack.
 *  Thoses "push" and "pop" operation cannot be made explicitly, but instead needs to be done
 * through the MatrixStackPush "RAII" interface, for safety reason. 
 */
class MatrixStack {
public:
    /**
     * @brief   RAII Push/Pop interface to the MatrixStack.
     * @ingroup Main
     *
     *  The Push constructor simply save the current matrix onto the stack to allow for further modification to it,
     * then when the end of the scope is reached, or exception occurs, the Push destructor "pop" the stack
     * to restore the current matrix as it was before.
     */
    class Push {
     public:
        /**
         * @brief Constructor simply "push" the current matrix onto the stack.
         *
         * @param[in] aMatrixStack  MatrixStack to operate
         */
        explicit Push(MatrixStack& aMatrixStack) :
            mMatrixStack(aMatrixStack) {
            mMatrixStack.push();
        }
        /**
         * @brief Destructor simply "pop" the stack to restore the current matrix as it was before the constructor.
         */
        ~Push() {
            mMatrixStack.pop();
        }

     private:
        MatrixStack& mMatrixStack;  ///< Stack of glm::mat4 matrices

     private:
        /// disallow copy constructor and assignment operator
        DISALLOW_COPY_AND_ASSIGN(Push);
    };

public:
    /**
     * @brief Constructor
     *
     * @param[in] aMatrix   Matrix to initialise the stack with
     */
    explicit MatrixStack(const glm::mat4 &aMatrix) :
        mCurrentMatrix(aMatrix) {
    }

    /**
     * @brief Destructor (not virtual because no virtual methods and class not derived)
     */
    ~MatrixStack() {
    }

    // Right multiply the current matrix with the one provided
    inline void             multiply(const glm::mat4 &aMatrix);

    // Get the current matrix
    inline const glm::mat4& top() const;
    inline       glm::mat4& top();

private:
    // "push" and "pop" operation need to be done through the MatrixStackPush "RAII" interface.
    inline void push();
    inline void pop();

private:
    typedef std::stack<glm::mat4> Stack;    ///< Stack of glm::mat4 matrices

private:
    Stack       mStack;         ///< Stack of glm::mat4 matrices
    glm::mat4   mCurrentMatrix; ///< Current glm::mat4 matrix

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(MatrixStack);
};

/**
 * @brief Right-multiply the current matrix with the one provided
 *
 * @param[in] aMatrix   Matrix to multiply the stack with
 */
inline void MatrixStack::multiply(const glm::mat4 &aMatrix) {
    mCurrentMatrix *= aMatrix;
}

/**
 * @brief Get the current matrix
 */
inline const glm::mat4& MatrixStack::top() const {
    return mCurrentMatrix;
}

/**
 * @brief Get the current matrix
 */
inline glm::mat4& MatrixStack::top() {
    return mCurrentMatrix;
}

/**
 * @brief Push the current matrix onto the stack to preserve it. Used by MatrixStackPush.
 */
inline void MatrixStack::push() {
    mStack.push(mCurrentMatrix);
}

/**
 * @brief Restore a previously saved matrix from the stack. Used by MatrixStackPush.
 */
inline void MatrixStack::pop() {
    mCurrentMatrix = mStack.top();
    mStack.pop();
}
