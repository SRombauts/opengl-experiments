/**
 * @file    App.cpp
 * @ingroup Main
 * @brief   Application managing the lifecycle of GLUT and the rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/App.h"

/**
 * @brief Constructor
 */
App::App() :
    mLog("App"),
    mInput(mRenderer) {
}
/**
 * @brief Destructor
 */
App::~App() {
}
