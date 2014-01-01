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

#include "OVR.h" // NOLINT(build/include): OculusVR fault!


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

private:
    Log::Logger                     mLog;           ///< Logger object to output runtime information
    OVR::System                     mSystem;        ///< LibOVR core system: needs to be Init before any OVR Kernel use
    OVR::Ptr<OVR::DeviceManager>    mManagerPtr;    ///< Device Manager provides access to devices supported by OVR
    OVR::Ptr<OVR::HMDDevice>        mHMDPtr;        ///< Represents an Oculus HMD device unit
    OVR::HMDInfo                    mHMDInfo;       ///< Describes the HMD allowing us to configure rendering
    OVR::Ptr<OVR::SensorDevice>     mSensorPtr;     ///< Sensors data interface of the HMD Device
    OVR::Ptr<OVR::Profile>          mUserProfilePtr; ///< Profile of the user (Eye height, IPD...)
    OVR::SensorFusion               mSensorFusion;  ///< Accumulates Gyro/Accelero/Magneto to keep track of orientation
    OVR::Util::Render::StereoConfig mStereoConfig;  ///< Stereo view parameters
};
