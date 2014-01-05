/**
 * @file    OculusHMD.h
 * @ingroup Main
 * @brief   Container for root Nodes of a hierarchical OculusHMD graph
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/Logger.h"

#include "Utils/shared_ptr.hpp"

#include "OVR.h" // NOLINT(build/include): OculusVR fault!

#include <glm/gtc/quaternion.hpp>       // glm::fquat

/**
 * @brief   Manage interface with the Oculus Head Mounted Display
 * @ingroup Main
 *
 * - Get device properties
 * - Get user configuration
 * - Access head orientation
 *
 * @todo OculusHMD need a "clean" interface without "OVR.h" to pass to the renderer
*/
class OculusHMD {
public:
    OculusHMD();
    ~OculusHMD();

    // Load default information if no real device found
    void fakeInfo();

    // Reset orientation of the HMD
    void resetOrientation();

    // Get quaternion of orientation of the HMD
    glm::fquat getOrientation() const;

private:
    typedef Utils::shared_ptr<OVR::SensorFusion> SensorFusionPtr;   ///< Share pointer of SensorFusion result object

    Log::Logger                     mLog;               ///< Logger object to output runtime information
    OVR::System                     mSystem;            //!< Init Oculus Core system (memory allocator, threads)
    OVR::HMDInfo                    mHMDInfo;           ///< Describes the HMD allowing us to configure rendering
    OVR::Ptr<OVR::SensorDevice>     mSensorPtr;         ///< Sensors data interface of the HMD Device
    SensorFusionPtr                 mSensorFusionPtr;   ///< Fusion Gyro/Accelero/Magneto to keep track of orientation
    OVR::Util::Render::StereoConfig mStereoConfig;      ///< Stereo view parameters
};
