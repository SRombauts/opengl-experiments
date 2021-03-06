/**
 * @file    OculusHMDImpl.cpp
 * @ingroup Main
 * @brief   Private interface with the Oculus Head Mounted Display.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/OculusHMDImpl.h"
#include "Utils/Exception.h"

#include <cassert>

/**
 * @brief Constructor - finds the Oculus Rift and its sensor, or throw en exception.
 */
OculusHMDImpl::OculusHMDImpl() :
    mLog("OculusHMD"),
    mPredictionLookaheadMs(30) {

    // Those vars are reference counted; no need to keep them around explicitly,
    // they will be freed when Sensor & SensorFusion will be cleared (in OculusHMDImpl destructor)
    OVR::Ptr<OVR::DeviceManager>    mManagerPtr;        ///< Device Manager provides access to devices supported by OVR
    OVR::Ptr<OVR::HMDDevice>        mHMDPtr;            ///< Represents an Oculus HMD device unit
    OVR::Ptr<OVR::Profile>          mUserProfilePtr;    ///< Profile of the user (Eye height, IPD...)

    // LibOVR core system: needs to be Initialized before any OVR Kernel use
//  OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All)); // done by mSystem constructor

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
                mStereoConfig.SetIPD(mUserProfilePtr->GetIPD());

                mLog.info() << "Calculated appropriate FOV: " << mStereoConfig.GetYFOVDegrees() << "deg";
                mLog.info() << "Calculated screen center offset: " << mStereoConfig.GetProjectionCenterOffset() << "m";

                // Access sensor interface of the HMD device
                mSensorPtr = *(mHMDPtr->GetSensor());
                if (mSensorPtr) {
                    // Create an object to accumulate data from Gyro/Accelero/Magneto to keep track of orientation
                    mSensorFusionPtr.reset(new OVR::SensorFusion());

                    // Attach Sensor to SensorFusion object for it to receive frame messages and update orientation.
                    mSensorFusionPtr->AttachToSensor(mSensorPtr);

                    // Enable prediction with the default delta of 30ms (0.03s)
                 // mSensorFusionPtr->SetPredictionEnabled(true);
                    setPrediction(mPredictionLookaheadMs);
                } else {
                    mLog.error() << "No sensor found";
                    throw Utils::Exception("error: No sensor found");
                }
                mUserProfilePtr.Clear();
            } else {
                mLog.error() << "No user profile";
                throw Utils::Exception("error: No user profile");
            }
        } else {
            mLog.error() << "No HMD info";
            throw Utils::Exception("error: No HMD info");
        }
        mHMDPtr.Clear();
    } else {
        mLog.notice() << "No HMD found";
        throw Utils::Exception("note: No HMD found");
    }
    mManagerPtr.Clear();
}

/**
 * @brief Destructor
 */
OculusHMDImpl::~OculusHMDImpl() {
    mSensorPtr.Clear();
    mSensorFusionPtr.reset();

//  OVR::System::Destroy(); // done by mSystem destructor
}

/**
 * @brief Load default HMD information to fake its presence for development purpose
 *
 * @note Not used any more
 */
void OculusHMDImpl::fakeInfo() {
    mHMDInfo.HResolution = 1280;
    mHMDInfo.VResolution = 800;
    mHMDInfo.HScreenSize = 0.149759993f;
    mHMDInfo.VScreenSize = 0.0935999975f;
    mHMDInfo.VScreenCenter = 0.0467999987f;
    mHMDInfo.EyeToScreenDistance    = 0.0410000011f;
    mHMDInfo.LensSeparationDistance = 0.0635000020f;
    mHMDInfo.InterpupillaryDistance = 0.0640000030f;
    mHMDInfo.DistortionK[0] = 1.00000000f;
    mHMDInfo.DistortionK[1] = 0.219999999f;
    mHMDInfo.DistortionK[2] = 0.239999995f;
    mHMDInfo.DistortionK[3] = 0.000000000f;
    mHMDInfo.ChromaAbCorrection[0] = 0.995999992f;
    mHMDInfo.ChromaAbCorrection[1] = -0.00400000019f;
    mHMDInfo.ChromaAbCorrection[2] = 1.01400006f;
    mHMDInfo.ChromaAbCorrection[3] = 0.000000000f;
    mHMDInfo.DesktopX = 0;
    mHMDInfo.DesktopY = 0;
}

/**
 * @brief Set prediction lookahead in miliseconds
 *
 * @param[in] aPredictionDeltaMs    prediction lookahead amount in miliseconds
 */
void OculusHMDImpl::setPrediction(int aPredictionDeltaMs) {
    assert(mSensorPtr && mSensorFusionPtr);

    mPredictionLookaheadMs = aPredictionDeltaMs;
    // Enable prediction with the provided delta (default is 30ms 0.03s)
    mSensorFusionPtr->SetPrediction(0.001f * aPredictionDeltaMs);
    mLog.info() << "SetPrediction(" << aPredictionDeltaMs << "ms) = " << (0.001f * aPredictionDeltaMs);
}

/**
 * @brief Increment/decrement prediction lookahead by some miliseconds
 *
 * @param[in] aOffset Offset to add/remove to the lookahead in miliseconds
 */
void OculusHMDImpl::incrPrediction(int aOffset) {
    if (100 > mPredictionLookaheadMs) {
        mPredictionLookaheadMs += aOffset;
        setPrediction(mPredictionLookaheadMs);
    }
}

/**
 * @brief Reset orientation of the HMD
 */
void OculusHMDImpl::resetOrientation() {
    assert(mSensorFusionPtr);

    mSensorFusionPtr->Reset();
}

/**
 * @brief Get quaternion of orientation of the HMD
 *
 * @return A GLM quaternion of orientation
 */
glm::fquat OculusHMDImpl::getOrientation() const {
    assert(mSensorFusionPtr);

    OVR::Quatf hmdOrientation = mSensorFusionPtr->GetOrientation();
    return glm::fquat(hmdOrientation.w, hmdOrientation.x, hmdOrientation.y, hmdOrientation.z);
}

