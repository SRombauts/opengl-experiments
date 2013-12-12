#!/bin/sh

echo "Getting dependencies..."
git submodule init
git submodule update
cd unofficial-opengl-sdk
git submodule init
git submodule update

echo "Generating freeglut..."
cd freeglut/freeglut
cmake . -DCMAKE_BUILD_TYPE=Debug
echo "Building freeglut..."
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_staticd.a
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_static.a
cd ../..

echo "Generating sdk..."
../premake/premake4 gmake
make -j4
cd ..

echo "Generating assimp..."
cd assimp
cmake . -DASSIMP_BUILD_STATIC_LIB=ON
echo "Building assimp..."
cmake --build . --config Debug
move lib/assimp.lib lib/assimpd.lib
cmake --build . --config Release
cd ..

