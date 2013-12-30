/**
 * @file    Timer.h
 * @ingroup Utils
 * @brief   Cyclic timing event generation
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
 * @brief Cyclic timing event generation
 * @ingroup Utils
 */
class Timer {
public:
    explicit Timer(time_t aIntervalUs);
    ~Timer(); // not virtual because no virtual methods and class not derived

    // Tell if the time elapsed since the last timer tick is enough
    bool isTimeElapsed(time_t aCurrentTickUs);

    // Getters
    inline time_t   getStartTickUs()    const;
    inline time_t   getElapsedTimeUs()  const;

private:
    time_t  mIntervalUs;        ///< Number of microseconds between Timer ticks

    time_t  mStartTickUs;       ///< Tick of the last timer tick, in microseconds
    time_t  mElapsedTimeUs;     ///< Time elapsed since the previous timer tick, in microseconds

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Timer);
};

/**
 * @brief Get tick of the last timer tick, in microseconds
 */
inline time_t Timer::getStartTickUs() const {
    return mStartTickUs;
}

/**
 * @brief Get time elapsed since the previous timer tick, in microseconds
 */
inline time_t Timer::getElapsedTimeUs() const {
    return mElapsedTimeUs;
}

} // namespace Utils
