# Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT) 

cmake_minimum_required(VERSION 2.6)
project(glExperiments)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    set(SYSTEM_LIBRARIES        opengl32.lib)
    # disable Visual Studio usual warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # For OculusVR / LibOVR debug build
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -DOVR_BUILD_DEBUG")
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")
    set(SYSTEM_LIBRARIES        GL Xrandr Xi)
    # GCC flags
    # TODO: how to define for some libs only
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fstack-protector-all -Wall -Wextra -pedantic -Weffc++ -Wformat-security -Winit-self -Wswitch-default -Wswitch-enum -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wconversion -Wlogical-op -Winline -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn)
    message(STATUS "C++11 activated for std::shared_ptr")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")  # -std=c++11
endif ()
set(CPPLINT_ARG_VERBOSE "--verbose=3")
set(CPPLINT_ARG_LINELENGTH "--linelength=120")


## Core source code ##

# All includes are relative to the "src" directory 
include_directories("${PROJECT_SOURCE_DIR}/src")

# adding a new file require explicittly modifing the CMakeLists.txt
# so that CMake knows that it should rebuild the project (it is best practice)

# list of sources files of the "Main" module
set(OPENGL_EXPERIMENTS_SRC_MAIN
 src/Main/App.h src/Main/App.cpp
 src/Main/Main.cpp
 src/Main/MatrixStack.h
 src/Main/Mesh.h src/Main/Mesh.cpp
 src/Main/Node.h src/Main/Node.cpp
 src/Main/OculusHMD.h src/Main/OculusHMD.cpp
 src/Main/Physic.h src/Main/Physic.cpp
 src/Main/Renderer.h src/Main/Renderer.cpp
 src/Main/Scene.h src/Main/Scene.cpp
 src/Main/ShaderProgram.h src/Main/ShaderProgram.cpp
)
source_group(Main FILES ${OPENGL_EXPERIMENTS_SRC_MAIN})

# list of sources files of the "Utils" module
set(OPENGL_EXPERIMENTS_SRC_UTILS
 src/Utils/Exception.h
 src/Utils/Formatter.h
 src/Utils/FPS.h src/Utils/FPS.cpp
 src/Utils/Measure.h
 src/Utils/shared_ptr.hpp
 src/Utils/String.h
 src/Utils/Time.h src/Utils/Time.cpp
 src/Utils/Timer.h src/Utils/Timer.cpp
 src/Utils/Utils.h
)
source_group(Utils FILES ${OPENGL_EXPERIMENTS_SRC_UTILS})

# list of "data" files ; source for vertex and fragment shaders
set(OPENGL_EXPERIMENTS_DATA
 data/ModelWorldCameraClip.vert
 data/PassthroughColor.frag
)
source_group(data FILES ${OPENGL_EXPERIMENTS_DATA})

# add sources for the executable :
add_executable(glExperiments
 ${OPENGL_EXPERIMENTS_SRC_MAIN}
 ${OPENGL_EXPERIMENTS_SRC_UTILS}
 ${OPENGL_EXPERIMENTS_DATA}
)


## Libraries ##

# add the subdirectory of the glfw static library (but disable any other build)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
set(GLFW_DOCUMENT_INTERNALS OFF CACHE BOOL "Include internals in documentation")
add_subdirectory(glfw)
include_directories("${PROJECT_SOURCE_DIR}/glfw/include")

# add the subdirectory of the glload static library
add_subdirectory(glload) 
include_directories("${PROJECT_SOURCE_DIR}/glload/include")

# add the subdirectory of the glm library (header only)
add_subdirectory(glm/glm)
add_definitions(-DGLM_FORCE_RADIANS) # use only modern API in radians
include_directories("${PROJECT_SOURCE_DIR}/glm")

# add the subdirectory of the assimp static library (but disable tool builds)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build a static (.a) version of the library")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
add_subdirectory(assimp)
include_directories("${PROJECT_SOURCE_DIR}/assimp/include")

# add the subdirectory of the LoggerCpp library (but disable any other build)
set(LOGGERCPP_BUILD_EXAMPLE OFF CACHE BOOL "Build the example of LoggerCpp.")
set(LOGGERCPP_RUN_CPPLINT  OFF CACHE BOOL "Run cpplint.py tool for Google C++ StyleGuide.")
set(LOGGERCPP_RUN_CPPCHECK OFF CACHE BOOL "Run cppcheck C++ static analysis tool.")
set(LOGGERCPP_RUN_DOXYGEN  OFF CACHE BOOL "Run Doxygen C++ documentation tool.")
add_subdirectory(LoggerCpp)
include_directories("${PROJECT_SOURCE_DIR}/LoggerCpp/include")

## Oculus SDK library in last position ##
# add the subdirectory of the Oculus SDK LibOVR library
if (MSVC)
    # Oculus LibOVR uses unicode (WCHAR/wchar_t) windows API
    # WARNING: this can affect any further library! Keep in last position.
    add_definitions(-DUNICODE -D_UNICODE)
endif ()
add_subdirectory(OculusSDK/LibOVR)
include_directories("${PROJECT_SOURCE_DIR}/OculusSDK/LibOVR/Include")


## Linking ##

# link the executable with all required libraries
target_link_libraries(glExperiments glfw glload assimp LoggerCpp OculusVR ${SYSTEM_LIBRARIES})


# Optional additional targets:

option(OPENGL_EXPERIMENTS_RUN_CPPLINT "Run cpplint.py tool for Google C++ StyleGuide." ON)
if (OPENGL_EXPERIMENTS_RUN_CPPLINT)
    # add a cpplint target to the "all" target
    add_custom_target(glExperiments_cpplint
     ALL
     COMMAND python cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_VERBOSE} ${CPPLINT_ARG_LINELENGTH} ${OPENGL_EXPERIMENTS_SRC_MAIN} ${OPENGL_EXPERIMENTS_SRC_UTILS}
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()

option(OPENGL_EXPERIMENTS_RUN_CPPCHECK "Run cppcheck C++ static analysis tool." ON)
if (OPENGL_EXPERIMENTS_RUN_CPPCHECK)
    # add a cppcheck target to the "all" target
    add_custom_target(glExperiments_cppcheck
     ALL
     COMMAND cppcheck -j 4 cppcheck --enable=style --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
    )
endif()

option(OPENGL_EXPERIMENTS_RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)
if (OPENGL_EXPERIMENTS_RUN_DOXYGEN)
    # add a Doxygen target to the "all" target
    add_custom_target(glExperiments_doxygen
     ALL
     COMMAND doxygen Doxyfile > ${DEV_NULL}
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()
