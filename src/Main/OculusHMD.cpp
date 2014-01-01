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
    mLog("OculusHMD"),
    mSystem(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All)) {

    // Access to the first Oculus HMD device found
// TODO THIS BLOCK the end of the application
    mManagerPtr = OVR::DeviceManager::Create();
    mHMDPtr = mManagerPtr->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
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
            mSensorPtr = mHMDPtr->GetSensor();
            if (mSensorPtr) {
                // We need to attach sensor to SensorFusion object for it to receive
                // body frame messages and update orientation. SFusion.GetOrientation()
                // is used in OnIdle() to orient the view.
// TODO Crash !
//              mSensorFusion.AttachToSensor(mSensorPtr);

                // Enable prediction with the default delta of 30ms (0.03s)
                mSensorFusion.SetPredictionEnabled(true);

                // TODO call periodically:

                // Extract Pitch, Yaw, Roll instead of directly using the orientation
                OVR::Quatf hmdOrient = mSensorFusion.GetOrientation();
                float pitch = 0.0f;
                float yaw = 0.0f;
                float roll = 0.0f;
                hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&pitch, &yaw, &roll);
                mLog.debug() << "Current Orientation pitch=" << pitch << ", yaw=" << yaw << ", roll=" << roll;
                // NOTE: We can get a matrix from orientation as follows:
                OVR::Matrix4f hmdMat(hmdOrient);
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
 * @brief Destructor
 */
OculusHMD::~OculusHMD() {
    mSensorPtr.Clear();
    mHMDPtr.Clear();
}

