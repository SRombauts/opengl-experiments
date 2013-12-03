/**
 * @file    Measure.h
 * @ingroup Utils
 * @brief   Encapsulate a microsecond time measurement.
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Utils/Time.h"

namespace Utils {

/**
 * @brief   Encapsulate a microsecond time measurement.
 * @ingroup Utils
 *
 * @see #Utils::Time
 */
class Measure {
public:
    /**
     * @brief Constructor measure and save the first time tick.
     */
    inline Measure();

    /**
     * @brief Re-measure and save the first time tick.
     */
    inline void restart();

    /**
     * @brief Measure a second time tick and return the difference since the first one.
     */
    inline time_t diff();

private:
    time_t mStartTimeUs;    ///< First time measurement, from the constructor.
};


// Constructor measure and save the first time tick.
inline Measure::Measure() {
    mStartTimeUs = Time::getTickUs();
}

// Re-measure and save the first time tick.
inline void Measure::restart() {
    mStartTimeUs = Time::getTickUs();
}

// Measure a second time tick and return the difference since the first one.
inline time_t Measure::diff() {
    return Time::diff(mStartTimeUs, Time::getTickUs());
}

} // namespace Utils
