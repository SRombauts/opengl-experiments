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

#include "LoggerCpp/LoggerCpp.h"

#include "Utils/Utils.h"

#include <ctime>

namespace Utils {

/**
 * @brief Management of the GLUT input callbacks
 */
class FPS {
public:
    explicit FPS();
    ~FPS(); // not virtual because no virtual methods and class not derived

    // Inter-frame timings and FPS calculation
    void calculate();

    // Getters
    inline time_t   getCurrentFrameTickUs() const;
    inline time_t   getElapsedTimeUs()      const;

private:
    Log::Logger         mLog;                   ///< Logger object to output runtime information

    time_t              mCurrentFrameTickUs;    ///< Tick of the current frame, in microseconds
    time_t              mElapsedTimeUs;         ///< Time elapsed since the previous frame, in microseconds

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(FPS);
};


/**
 * @brief Get tick of the current frame, in microseconds
 */
inline time_t FPS::getCurrentFrameTickUs() const{
    return mCurrentFrameTickUs;
}

/**
 * @brief Get time elapsed since the previous frame, in microseconds
 */
inline time_t FPS::getElapsedTimeUs() const {
    return mElapsedTimeUs;
}
    
} // namespace Utils
