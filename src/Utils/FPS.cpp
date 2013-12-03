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
 */
FPS::FPS() :
    mLog("FPS") {
}

/**
 * @brief Destructor
 */
FPS::~FPS() {
}

/**
 * @brief Calculate and print FPS, and average and word frame duration
 */
void FPS::calculate() {
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
    if (totalUs >= 1000000) {
        time_t avgUs = totalUs/_nbFrames;
        mLog.notice() << _nbFrames << "fps (avg " << avgUs/1000 << "." << avgUs%1000
                      << "ms, worst " << _worstFrameUs/1000 << "." << _worstFrameUs%1000 << "ms)";
        _firstTickUs =  Utils::Time::getTickUs(); // do not count logging time
        _nbFrames = 0;
        _worstFrameUs = 0;
    }
}

} // namespace Utils
