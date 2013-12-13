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
make clean
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_staticd.a
make clean
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_static.a
cd ../..

echo "Generating sdk..."
../premake/premake4 gmake
make -j4 config=debug
make -j4 config=release
cd ..

echo "Generating assimp..."
cd assimp
cmake . -DASSIMP_BUILD_STATIC_LIB=ON
echo "Building assimp..."
make clean
cmake --build . --config Debug
make clean
mv lib/libassimp.a lib/libassimpd.a
cmake --build . --config Release
cd ..

