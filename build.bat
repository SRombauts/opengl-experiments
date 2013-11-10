@echo "Getting dependencies..."
git submodule init
git submodule update --recursive

@echo "Building freeglut..."
cd unofficial-opengl-sdk\freeglut\freeglut
cmake . -G "Visual Studio 10"
cmake --build . --target freeglut_static --config Debug
move lib\Debug\* lib
cmake --build . --target freeglut_static --config Release
move lib\Release\* lib
cd ..\..
..\premake\premake4 vs2010
@echo "[open and build solution]"
cd ..
premake\premake4 vs2010
@echo "[open and build solution]"
