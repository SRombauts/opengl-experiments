/**
 * @file    OculusHMD.cpp
 * @ingroup Main
 * @brief   Container for root Nodes of a hierarchical OculusHMD graph
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/OculusHMD.h"

/**
 * @brief Constructor
 */
OculusHMD::OculusHMD() :
    mLog("OculusHMD") {

    // LibOVR core system: needs to be Initialized before any OVR Kernel use
    // (NOTE, using a mSystem member variable to automate this does crash VS debugger at shutdown)
    OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

    // Access to the first Oculus HMD device found
    mManagerPtr = *(OVR::DeviceManager::Create());
    mHMDPtr = *(mManagerPtr->EnumerateDevices<OVR::HMDDevice>().CreateDevice());
    if (mHMDPtr) {
        // Get detailed information about the device and its capabilities
        bool bHasInfo = mHMDPtr->GetDeviceInfo(&mHMDInfo);
        if (bHasInfo) {
            mStereoConfig.SetHMDInfo(mHMDInfo);
            mLog.info() << "HMD '" << mHMDInfo.ProductName << "' found, on screen '"
                        << mHMDInfo.DisplayDeviceName << "' ("
                        << mHMDInfo.HResolution << "x" << mHMDInfo.VResolution << ")";

            // Retrieve user profile settings
            mUserProfilePtr = mHMDPtr->GetProfile();
            if (mUserProfilePtr) {
                mLog.info() << "User profile Height=" << mUserProfilePtr->GetPlayerHeight()
                            << " EyeHeight=" << mUserProfilePtr->GetEyeHeight()
                            << " IPD=" << mUserProfilePtr->GetIPD();

                // Access sensor interface of the HMD device
                mSensorPtr = *(mHMDPtr->GetSensor());
                if (mSensorPtr) {
                    // Create an object to accumulate data from Gyro/Accelero/Magneto to keep track of orientation
                    mSensorFusionPtr.reset(new OVR::SensorFusion());

                    // Attach Sensor to SensorFusion object for it to receive frame messages and update orientation.
                    mSensorFusionPtr->AttachToSensor(mSensorPtr);

                    // Enable prediction with the default delta of 30ms (0.03s)
                    mSensorFusionPtr->SetPredictionEnabled(true);
                }
            } else {
                mLog.error() << "No user profile";
            }
        } else {
            mLog.error() << "No HMD info";
        }
    } else {
        mLog.notice() << "No HMD found";
    }
}

/**
 * @brief Reset orientation of the HMD
 */
void OculusHMD::resetOrientation() {
    mSensorFusionPtr->Reset();
}

/**
 * @brief Get quaternion of orientation of the HMD
 *
 * @return A GLM quaternion of orientation
 *
 * @todo Refactor interface to avoid this double return
 */
glm::fquat OculusHMD::getOrientation() const {
    if (mSensorFusionPtr) {
        OVR::Quatf hmdOrientation = mSensorFusionPtr->GetOrientation();
        return glm::fquat(hmdOrientation.w, hmdOrientation.x, hmdOrientation.y, hmdOrientation.z);
    } else {
        return glm::fquat();
    }
}

/**
 * @brief Destructor
 */
OculusHMD::~OculusHMD() {
    mUserProfilePtr.Clear();
    mSensorPtr.Clear();
    mHMDPtr.Clear();
    mManagerPtr.Clear();

    mSensorFusionPtr.reset();

    OVR::System::Destroy();
}

