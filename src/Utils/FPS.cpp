/**
 * @file    FPS.cpp
 * @ingroup Utils
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 *
 * Copyright (c) 2013-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Utils/FPS.h"

namespace Utils {

/**
 * @brief Constructor
 *
 * @param[in] aCalculationInterval    Number of microseconds to take into account for FPS calculation
 */
FPS::FPS(float aCalculationInterval) :
    mCalculationInterval(aCalculationInterval),
    mStartFrameTime(0),
    mElapsedTime(0),
    mCalculatedFPS(0.0f),
    mAverageInterFrame(0),
    mWorstInterFrame(0) {
}

/**
 * @brief Destructor
 */
FPS::~FPS() {
}

/**
 * @brief Calculate and print FPS, and average and word frame duration
 *
 * @param[in] aStartFrameTime   Time before the start of the rendering
 *
 * @return true if a new FPS value is available
 */
bool FPS::start(double aStartFrameTime) {
    bool           bNewCalculatedFPS = false;
    static int     _nbFrames    = 0;
    static float   _worstFrame   = 0;
    static double  _firstTime    = aStartFrameTime;
    static double  _prevTime     = aStartFrameTime;
    double         curTime       = aStartFrameTime;
    float          total         = static_cast<float>(curTime - _firstTime);
    float          frame         = static_cast<float>(curTime - _prevTime);

    _prevTime = curTime;
    ++_nbFrames;

    if (frame > _worstFrame) {
        _worstFrame = frame;
    }

    // Save some useful values
    mStartFrameTime     = curTime;
    mElapsedTime        = frame;
    mWorstInterFrame    = _worstFrame;

    if (total >= mCalculationInterval) {
        mCalculatedFPS      = _nbFrames/total;
        bNewCalculatedFPS   = true;
        mAverageInterFrame = total/_nbFrames;
        _firstTime = curTime;
        _nbFrames = 0;
        _worstFrame = 0;
    }

    return bNewCalculatedFPS;
}

/**
 * @brief   Measure frame render-time, since calculate() was called
 *
 * @param[in] aEndRenderTime    Time the end of the rendering
 */
void FPS::end(double aEndRenderTime) {
    mLastRenderTime = static_cast<float>(aEndRenderTime - mStartFrameTime);
}

} // namespace Utils
