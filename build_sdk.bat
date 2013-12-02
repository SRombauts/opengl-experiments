@echo "Getting dependencies..."
git submodule init
git submodule update
cd unofficial-opengl-sdk
git submodule init
git submodule update

@echo "Building freeglut..."
cd freeglut\freeglut
cmake . -G "Visual Studio 10"
cmake --build . --target freeglut_static --config Debug
move lib\Debug\* lib
cmake --build . --target freeglut_static --config Release
move lib\Release\* lib
cd ..\..

@echo "Generating sdk..."
..\premake\premake4 vs2010
@echo "[open and build solution]"
cd ..

