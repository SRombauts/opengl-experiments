/**
 * @file    FPS.cpp
 * @ingroup Utils
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Utils/FPS.h"

#include "Utils/Time.h"

namespace Utils {

/**
 * @brief Constructor
 *
 * @param[in] aCalculationIntervalUs    Number of microseconds to take into account for FPS calculation
 */
FPS::FPS(time_t aCalculationIntervalUs) :
    mCalculationIntervalUs(aCalculationIntervalUs),
    mCurrentFrameTickUs(0),
    mElapsedTimeUs(0),
    mCalculatedFPS(0.0f),
    mAverageInterFrameUs(0),
    mWorstInterFrameUs(0) {
}

/**
 * @brief Destructor
 */
FPS::~FPS() {
}

/**
 * @brief Calculate and print FPS, and average and word frame duration
 *
 * @return true if a new FPS value is available
 */
bool FPS::calculate() {
    bool            bNewCalculatedFPS = false;
    static int      _nbFrames       = 0;
    static time_t   _worstFrameUs   = 0;
    static time_t   _firstTickUs    = Utils::Time::getTickUs();
    static time_t   _prevTickUs     = Utils::Time::getTickUs();
    time_t          curTickUs       = Utils::Time::getTickUs();
    time_t          totalUs         = (curTickUs - _firstTickUs);
    time_t          frameUs         = (curTickUs - _prevTickUs);

    _prevTickUs = curTickUs;
    ++_nbFrames;

    if (frameUs > _worstFrameUs) {
        _worstFrameUs = frameUs;
    }

    // Save some useful values
    mCurrentFrameTickUs = curTickUs;
    mElapsedTimeUs      = frameUs;
    mWorstInterFrameUs  = _worstFrameUs;

    if (totalUs >= mCalculationIntervalUs) {
        mCalculatedFPS      = static_cast<float>(_nbFrames)*1000000/totalUs;
        bNewCalculatedFPS   = true;
        mAverageInterFrameUs = totalUs/_nbFrames;
        _firstTickUs = curTickUs;
        _nbFrames = 0;
        _worstFrameUs = 0;
    }

    return bNewCalculatedFPS;
}

} // namespace Utils
