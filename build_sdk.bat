@echo "Getting dependencies..."
git submodule init
git submodule update
cd unofficial-opengl-sdk
git submodule init
git submodule update

@echo "Generating freeglut..."
cd freeglut\freeglut
cmake . -G "Visual Studio 10"
@echo "Building freeglut..."
cmake --build . --target freeglut_static --config Debug
move lib\Debug\* lib
cmake --build . --target freeglut_static --config Release
move lib\Release\* lib
cd ..\..

@echo "Generating sdk..."
..\premake\premake4 vs2010
@echo "[open and build solution]"
cd ..

@echo "Generating assimp..."
cd assimp
cmake . -G "Visual Studio 10" -DASSIMP_BUILD_STATIC_LIB=ON
@echo "Building assimp..."
cmake --build . --config Debug
move lib\Debug\* lib
cmake --build . --config Release
move lib\Release\* lib
cd ..

