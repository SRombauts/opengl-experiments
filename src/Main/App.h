/**
 * @file    App.h
 * @ingroup Main
 * @brief   Application managing the lifecycle of GLUT and the rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "LoggerCpp/LoggerCpp.h"

#include "Main/Input.h"
#include "Main/Renderer.h"

#include "Utils/Utils.h"

#include "Main/OculusHMD.h"

/**
 * @brief Application managing the lifecycle of GLUT and the rendering
 */
class App {
public:
    App();
    ~App();

private:
    Log::Logger mLog;       ///< Logger object to output runtime information

    Renderer    mRenderer;  ///< Manage OpenGL rendering
    Input       mInput;     ///< Manage GLUT input callbacks

    // @todo OculusHMD need a "clean" interface without "OVR.h" to pass to the renderer
//  OculusHMD   mOculusHMD; ///< Manage Oculus Head Mounted Display inputs

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(App);
};

