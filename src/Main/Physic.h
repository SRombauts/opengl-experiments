/**
 * @file    Physic.h
 * @ingroup Main
 * @brief   Physical properties of a #Node
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
#include <string>                       // std::string

/**
 * @brief Physical properties of a Node
 * @ingroup Main
 */
class Physic {
public:
    Physic();
    ~Physic();

    // Setters & Getters
    inline void setLinearSpeed(const glm::vec3& aLinearSpeed);
    inline void setRotationalSpeed(const glm::vec3& aRotationalSpeed);
    inline const glm::vec3& getLinearSpeed() const;
    inline const glm::vec3& getRotationalSpeed() const;
    inline bool             isInMotion() const;

private:
    bool        mbInMotion;         ///< true if Node as any kind of speed
    glm::vec3   mLinearSpeed;       ///< Linear movement
    glm::vec3   mRotationalSpeed;   ///< Rotational movement
};


/**
 * @brief   Set the linear speed of the Node
 *
 * @param[in] aLinearSpeed  3D vector whith the new translationnal speed
 */
inline void Physic::setLinearSpeed(const glm::vec3& aLinearSpeed) {
    mbInMotion = ( (0 != aLinearSpeed.x) || (0 != aLinearSpeed.y) || (0 != aLinearSpeed.z) );
    mLinearSpeed = aLinearSpeed;
}

/**
 * @brief   Set the linear speed of the Node
 *
 * @param[in] aRotationalSpeed  3D vector whith the new rotational speed
 */
inline void Physic::setRotationalSpeed(const glm::vec3& aRotationalSpeed) {
    mbInMotion = ( (0 != aRotationalSpeed.x) || (0 != aRotationalSpeed.y) || (0 != aRotationalSpeed.z) );
    mRotationalSpeed = aRotationalSpeed;
}

/**
 * @brief   Get the linear speed of the Node
 *
 * @return  Const 3D vector whith the translationnal speed
 */
inline const glm::vec3& Physic::getLinearSpeed() const {
    return mLinearSpeed;
}

/**
 * @brief   Get the rotational speed of the Node
 *
 * @return  Const 3D vector whith the translationnal speed
 */
inline const glm::vec3& Physic::getRotationalSpeed() const {
    return mRotationalSpeed;
}

/**
 * @brief   Tell if Node as any kind of speed
 *
 * @return  true if Node as any kind of speed
 */
inline bool Physic::isInMotion() const {
    return mbInMotion;
}
