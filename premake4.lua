
dofile("./unofficial-opengl-sdk/links.lua")

solution "OpenGL"
    configurations { "Debug", "Release" }
    defines {"_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE", "_SCL_SECURE_NO_WARNINGS", "TIXML_USE_STL"}
    defines {"FREEGLUT_STATIC"}

project("LoggerCpp")
    kind "StaticLib"
    language "c++"
    targetdir "LoggerCpp/lib"

    includedirs "LoggerCpp/include/"
    files {"LoggerCpp/src/*.cpp",
           "LoggerCpp/include/LoggerCpp/*.h",
           "LoggerCpp/include/LoggerCpp/*.hpp"}
	
    configuration "windows"
        defines "WIN32"
	
    configuration "Debug"
        defines {"DEBUG", "_DEBUG"};
        objdir "Debug";
        flags "Symbols";
        targetsuffix "d"

    configuration "Release"
        defines {"NDEBUG", "RELEASE"};
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"};
        objdir "Release";

project "OpenGL"
    kind "ConsoleApp"
    language "c++"

    files {"README.md", "LICENSE.txt", "premake4.lua"}
    files {"src/Main/*.cpp", "src/Main/*.h"}
    files {"src/Utils/*.cpp", "src/Utils/*.h", "src/Utils/*.hpp"}
    files {"data/*.vert", "data/*.frag"}
    includedirs "src/"
    
    includedirs "LoggerCpp/include/"
    libdirs "LoggerCpp/lib"
    
    includedirs "assimp/include/"
    libdirs "assimp/lib"
    
    UseLibs {"glload", "freeglut", "glm"}
    
    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        links {"zlibstatic"}
    
    configuration "linux"
        links {"GL", "GLU", "Xrandr", "Xi"}
    
    configuration "Debug"
        defines {"DEBUG", "_DEBUG"}
        targetsuffix "d"
        flags "Symbols"
        links {"assimpd", "LoggerCppd"}

    configuration "Release"
        defines {"RELEASE", "NDEBUG"}
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"};
        links {"assimp", "LoggerCpp"}

    configuration "linux"
        links {"z"}
        
