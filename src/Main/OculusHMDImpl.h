/**
 * @file    OculusHMDImpl.h
 * @ingroup Main
 * @brief   Private interface with the Oculus Head Mounted Display.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/Logger.h"

#include <memory>                   // std::unique_ptr

#include "OVR.h" // NOLINT(build/include): OculusVR fault!

#include <glm/gtc/quaternion.hpp>   // glm::fquat

/**
 * @brief   Private interface with the Oculus Head Mounted Display.
 * @ingroup Main
 *
 * Private Implémentation (PIMPL) is a way to hide the "OVR.h" include file and all its redefined types.
 *
 * @see OculusHMD class for detailed information
*/
class OculusHMDImpl {
public:
    OculusHMDImpl();
    ~OculusHMDImpl();

    // Load default information if no real device found
    void fakeInfo();

    // Set prediction lookahead amount in ms
    void setPrediction(int aPredictionDeltaMs);
    void incrPrediction(int aOffset);

    // Reset orientation of the HMD
    void resetOrientation();

    // Get quaternion of orientation of the HMD
    glm::fquat getOrientation() const;

private:
    typedef std::unique_ptr<OVR::SensorFusion>  SensorFusionPtr;    ///< Share pointer of SensorFusion result object

    Log::Logger                     mLog;               ///< Logger object to output runtime information
    OVR::System                     mSystem;            //!< Init Oculus Core system (memory allocator, threads)
    OVR::HMDInfo                    mHMDInfo;           ///< Describes the HMD allowing us to configure rendering
    OVR::Ptr<OVR::SensorDevice>     mSensorPtr;         ///< Sensors data interface of the HMD Device
    SensorFusionPtr                 mSensorFusionPtr;   ///< Fusion Gyro/Accelero/Magneto to keep track of orientation
    OVR::Util::Render::StereoConfig mStereoConfig;      ///< Stereo view parameters

    unsigned int                    mPredictionLookaheadMs; ///< Prediction Lookahead in milliseconds (default is 30ms)
};
