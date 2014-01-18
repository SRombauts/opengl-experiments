/**
 * @file    FPS.h
 * @ingroup Utils
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 *
 * Copyright (c) 2013-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/Utils.h"

#include <ctime>

namespace Utils {

/**
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 * @ingroup Utils
 */
class FPS {
public:
    explicit FPS(float aCalculationInterval);
    ~FPS(); // not virtual because no virtual methods and class not derived

    // Inter-frame timings and FPS calculation, calculated at the start of a frame
    bool start(double aStartFrameTime);
    // Measure frame render-time, since start() was called, at the end of the rendering
    void end(double aEndRenderTime);

    // Getters
    inline double getStartFrameTime()    const;
    inline float getElapsedTime()       const;
    inline float getCalculatedFPS()     const;
    inline float getAverageInterFrame() const;
    inline float getWorstInterFrame()   const;
    inline float getLastRenderTime()    const;

private:
    const float mCalculationInterval;   ///< Configured duration between FPS calculations

    // Inter-frame timing calculations
    double  mStartFrameTime;            ///< Time of the beginning of the current frame
    float   mElapsedTime;               ///< Time elapsed since the previous frame
    float   mCalculatedFPS;             ///< Frame-Per-Second value calculated during the last second
    float   mAverageInterFrame;         ///< Average inter-frame time during the last second
    float   mWorstInterFrame;           ///< Worst inter-frame time during the last second

    // Render time calculation
    float   mLastRenderTime;            ///< Duration of the last frame rendering

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(FPS);
};


/**
 * @brief Get tick of the current frame
 */
inline double FPS::getStartFrameTime() const {
    return mStartFrameTime;
}

/**
 * @brief Get time elapsed since the previous frame
 */
inline float FPS::getElapsedTime() const {
    return mElapsedTime;
}

/**
 * @brief Get the Frame-Per-Second value calculated during the last second
 */
inline float FPS::getCalculatedFPS() const {
    return mCalculatedFPS;
}

/**
 * @brief Get the average inter-frame time during the last second
 */
inline float FPS::getAverageInterFrame() const {
    return mAverageInterFrame;
}

/**
 * @brief Get the worst inter-frame time during the last second
 */
inline float FPS::getWorstInterFrame() const {
    return mWorstInterFrame;
}

/**
 * @brief Get the duration of the last frame rendering
 */
inline float FPS::getLastRenderTime() const {
    return mLastRenderTime;
}

} // namespace Utils
