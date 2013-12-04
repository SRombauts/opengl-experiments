/**
 * @file    Timer.cpp
 * @ingroup Utils
 * @brief   Frame-Per-Seconds and inter-frame timing calculation
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Utils/Timer.h"

#include "Utils/Time.h"

namespace Utils {

/**
 * @brief Constructor
 *
 * @param[in] aIntervalUs   Number of microseconds to take into account for Timer calculation
 */
Timer::Timer(time_t aIntervalUs) :
    mElapsedTimeUs(0),
    mIntervalUs(aIntervalUs) {
    mStartTickUs = Utils::Time::getTickUs();
}

/**
 * @brief Destructor
 */
Timer::~Timer() {
}

/**
 * @brief Tell if the time elapsed since the last timer tick is enough
 *
 * @param[in] aCurrentTickUs    Tick of the current frame in microseconds
 *
 * @return true if a new timer tick has been reached
 */
bool Timer::isTimeElapsed(time_t aCurrentTickUs) {
    bool    bNewTimerTick = false;
    time_t  elapsedTimeUs = (aCurrentTickUs - mStartTickUs);

    if (elapsedTimeUs >= mIntervalUs) {
        bNewTimerTick = true;
        // TODO SRombauts manage overflow => if fps is not enough to maintain accuracy
        mStartTickUs += mIntervalUs;
    }

    return bNewTimerTick;
}

} // namespace Utils
