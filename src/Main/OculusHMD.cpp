/**
 * @file    OculusHMD.cpp
 * @ingroup Main
 * @brief   Interface with the Oculus Head Mounted Display
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/OculusHMD.h"
#include "Main/OculusHMDImpl.h"

/**
 * @brief Constructor
 */
OculusHMD::OculusHMD() {
    try {
        // Create a private implementation of the interface
        mImplPtr.reset(new OculusHMDImpl());
    } catch (std::exception&) {
        // Error, no Oculus Rift found
    }
}

/**
 * @brief Destructor
 */
OculusHMD::~OculusHMD() {
    // mImplPtr is released
}

/**
 * @brief Set prediction lookahead in milliseconds
 *
 * @param[in] aPredictionDeltaMs    prediction lookahead amount in milliseconds
 */
void OculusHMD::setPrediction(int aPredictionDeltaMs) {
    if (mImplPtr) {
        mImplPtr->setPrediction(aPredictionDeltaMs);
    }
}

/**
 * @brief Increment/decrement prediction lookahead by some milliseconds
 *
 * @param[in] aOffset Offset to add/remove to the lookahead in milliseconds
 */
void OculusHMD::incrPrediction(int aOffset) {
    if (mImplPtr) {
        mImplPtr->incrPrediction(aOffset);
    }
}

/**
 * @brief Reset orientation of the HMD
 */
void OculusHMD::resetOrientation() {
    if (mImplPtr) {
        mImplPtr->resetOrientation();
    }
}

/**
 * @brief Get quaternion of orientation of the HMD
 *
 * @return A GLM quaternion of orientation
 *
 * @todo Refactor interface to avoid this double return
 */
glm::fquat OculusHMD::getOrientation() const {
    if (mImplPtr) {
        return mImplPtr->getOrientation();
    } else {
        return glm::fquat();
    }
}

