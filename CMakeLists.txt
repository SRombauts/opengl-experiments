# Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT) 

cmake_minimum_required(VERSION 2.6)
project(OpenGL)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    set(SYSTEM_LIBRARIES        "")
    # disable Visual Studio usual warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # For Oculus LibOVR debug build
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -DOVR_BUILD_DEBUG")
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")
    set(SYSTEM_LIBRARIES        "GL GLU Xrandr Xi")
    # GCC flags
    add_definitions(-rdynamic -fstack-protector-all -Wall -Wextra -pedantic -Weffc++ -Wformat-security -Winit-self -Wswitch-default -Wswitch-enum -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wconversion -Wlogical-op -Winline -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn)
    add_definitions(-std=c++0x)  # -std=c++11
endif ()
set(CPPLINT_ARG_VERBOSE "--verbose=3")
set(CPPLINT_ARG_LINELENGTH "--linelength=120")

## OpenGL Experiments ##

# add the subdirectory of the sources
include_directories("${PROJECT_SOURCE_DIR}/src")

# add sources for the executable
add_executable(OpenGL
 src/Main/App.h src/Main/App.cpp
 src/Main/Input.h src/Main/Input.cpp
 src/Main/Main.cpp
 src/Main/MatrixStack.h
 src/Main/Mesh.h src/Main/Mesh.cpp
 src/Main/Node.h src/Main/Node.cpp
 src/Main/Physic.h src/Main/Physic.cpp
 src/Main/Renderer.h src/Main/Renderer.cpp
 src/Main/Scene.h src/Main/Scene.cpp
 src/Main/ShaderProgram.h src/Main/ShaderProgram.cpp
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

## Libraries ##

# add the subdirectory of the freeglut static library (but disable any other build)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "Build FreeGLUT shared library.")
set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL       "Build FreeGLUT demos.")
add_definitions(-DFREEGLUT_STATIC)
add_subdirectory(freeglut/freeglut)
include_directories("${PROJECT_SOURCE_DIR}/freeglut/freeglut/include")

# add the subdirectory of the glload static library
add_subdirectory(glload) 
include_directories("${PROJECT_SOURCE_DIR}/glload/include")

# add the subdirectory of the glm library (header only)
add_subdirectory(glm/glm)
include_directories("${PROJECT_SOURCE_DIR}/glm")

# add the subdirectory of the assimp static library (but disable tool builds)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build a static (.a) version of the library")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
add_definitions(-DFREEGLUT_STATIC -DASSIMP_BUILD_STATIC_LIB=ON)
add_subdirectory(assimp)
include_directories("${PROJECT_SOURCE_DIR}/assimp/include")

# add the subdirectory of the LoggerCpp library (but disable any other build)
set(LOGGERCPP_RUN_CPPLINT  OFF CACHE BOOL "Run cpplint.py tool for Google C++ Styleguide.")
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
include_directories("${PROJECT_SOURCE_DIR}/OculusSDK/LibOVR/include")


## Linking ##

# link the executable with all required libraries
target_link_libraries(OpenGL freeglut_static glload assimp LoggerCpp OculusVR)


## Other Tools ##

# List all sources/headers files for cpplint:
# adding a file still require explicittly modifing the CMakeLists.txt
# so that CMake know that it should rebuild the project (it is best practice)
file(GLOB all_source_files
    "${PROJECT_SOURCE_DIR}/src/Main/*.*"
    "${PROJECT_SOURCE_DIR}/src/Utils/*.*"
)

# add a cpplint target to the "all" target
add_custom_target(OpenGL_cpplint
 ALL
 COMMAND python cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_VERBOSE} ${CPPLINT_ARG_LINELENGTH} ${all_source_files}
 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# add a cppcheck target to the "all" target
add_custom_target(OpenGL_cppcheck
 ALL
 COMMAND cppcheck -j 4 cppcheck --enable=style --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
)

# add a Doxygen target to the "all" target
add_custom_target(OpenGL_doxygen
 ALL
 COMMAND doxygen Doxyfile > ${DEV_NULL}
 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
