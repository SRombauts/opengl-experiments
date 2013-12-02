#!/bin/sh

echo "Getting dependencies..."
git submodule init
git submodule update
cd unofficial-opengl-sdk
git submodule init
git submodule update

echo "Building freeglut..."
cd freeglut/freeglut
cmake . -DCMAKE_BUILD_TYPE=Debug
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

