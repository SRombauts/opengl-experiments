
dofile("./unofficial-opengl-sdk/links.lua")

solution "OpenGL"
    configurations { "Debug", "Release" }
    defines {"_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE", "_SCL_SECURE_NO_WARNINGS", "TIXML_USE_STL"}
    defines {"FREEGLUT_STATIC"}
    
project "OpenGL"
    kind "ConsoleApp"
    language "c++"

    files {"README.md", "LICENSE.txt", "premake4.lua"}
    files {"src/*.cpp", "src/*.h"}
    files {"data/*.vert", "data/*.frag"}
    
    UseLibs {"glload", "freeglut", "glutil", "glm"}
    
    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
         
    configuration "linux"
        links {"GL", "GLU"}
    
    configuration "Debug"
        defines {"DEBUG", "_DEBUG"}
        targetsuffix "d"
        flags "Symbols"

    configuration "Release"
        defines {"RELEASE", "NDEBUG"}
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"};
