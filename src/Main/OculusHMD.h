/**
 * @file    OculusHMD.h
 * @ingroup Main
 * @brief   Interface with the Oculus Head Mounted Display.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/shared_ptr.hpp"

#include <glm/gtc/quaternion.hpp>       // glm::fquat


/// Private Implémentation (PIMPL) of the interface, hiding the "OVR.h" include file and all its redefined types.
class OculusHMDImpl;


/**
 * @brief   Interface with the Oculus Head Mounted Display.
 * @ingroup Main
 *
 * - Get device properties
 * - Get user configuration
 * - Access head orientation
*/
class OculusHMD {
public:
    OculusHMD();
    ~OculusHMD();

    // Set prediction lookahead amount in ms
    void setPrediction(int aPredictionDeltaMs);
    void incrPrediction(int aOffset);

    // Reset orientation of the HMD
    void resetOrientation();

    // Get quaternion of orientation of the HMD
    glm::fquat getOrientation() const;

private:
    Utils::shared_ptr<OculusHMDImpl>    mImplPtr;   ///< Opaque pointer to the Private Implementation (PIMPL)
};
