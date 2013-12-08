/**
 * @file    FPS.h
 * @ingroup Utils
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/Utils.h"

#include <ctime>

namespace Utils {

/**
 * @brief Frame-Per-Seconds and inter-frame timing calculation
 */
class FPS {
public:
    explicit FPS(time_t aCalculationIntervalUs);
    ~FPS(); // not virtual because no virtual methods and class not derived

    // Inter-frame timings and FPS calculation
    bool calculate();

    // Getters
    inline time_t   getCurrentFrameTickUs()     const;
    inline time_t   getElapsedTimeUs()          const;
    inline float    getCalculatedFPS()          const;
    inline time_t   getAverageInterFrameUs()    const;
    inline time_t   getWorstInterFrameUs()      const;

private:
    time_t  mCalculationIntervalUs; ///< Number of microseconds between FPS calculations

    time_t  mCurrentFrameTickUs;    ///< Tick of the current frame, in microseconds
    time_t  mElapsedTimeUs;         ///< Time elapsed since the previous frame, in microseconds
    float   mCalculatedFPS;         ///< Frame-Per-Second value calculated during the last second
    time_t  mAverageInterFrameUs;   ///< Average inter-frame time during the last second, in microseconds
    time_t  mWorstInterFrameUs;     ///< Worst inter-frame time during the last second, in microseconds

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(FPS);
};


/**
 * @brief Get tick of the current frame, in microseconds
 */
inline time_t FPS::getCurrentFrameTickUs() const {
    return mCurrentFrameTickUs;
}

/**
 * @brief Get time elapsed since the previous frame, in microseconds
 */
inline time_t FPS::getElapsedTimeUs() const {
    return mElapsedTimeUs;
}

/**
 * @brief Get the Frame-Per-Second value calculated during the last second
 */
inline float FPS::getCalculatedFPS() const {
    return mCalculatedFPS;
}

/**
 * @brief Get the average inter-frame time during the last second, in microseconds
 */
inline time_t FPS::getAverageInterFrameUs() const {
    return mAverageInterFrameUs;
}

/**
 * @brief Get the worst inter-frame time during the last second, in microseconds
 */
inline time_t FPS::getWorstInterFrameUs() const {
    return mWorstInterFrameUs;
}

} // namespace Utils
