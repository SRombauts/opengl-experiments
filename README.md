OpenGL Experiments in C++
-------------------------

### License

Copyright (c) 2012 Sébastien Rombauts (sebastien.rombauts@gmail.com)

Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
or copy at http://opensource.org/licenses/MIT)

### Getting and Building the dependencies

1. Get the dependencies recursively (premake, unofficial-opengl-sdk, freeglut, glm)

```bash
git submodule init
git submodule update --recursive
```

2. Build freeglut in the unofficial-opengl-sdk

On Linux :

```bash
sudo apt-get install libxrandr-dev libxi-dev

cd unofficial-opengl-sdk/freeglut/freeglut
cmake . -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_staticd.a
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --target freeglut_static
mv lib/libglut.a lib/libfreeglut_static.a
```

On Windows :

```bash
cd unofficial-opengl-sdk\freeglut\freeglut
cmake ..    # cmake .. -G "Visual Studio 10"
cmake --build . --target freeglut_static --config Debug
move lib\Debug\* lib
cmake --build . --target freeglut_static --config Release
move lib\Release\* lib
```

3. Building the other libs :
On Linux :

```bash
sudo apt-get install libxrandr-dev libxi-dev

cd unofficial-opengl-sdk
../premake/premake4 gmake
make
```

On Windows :

```bash
cd unofficial-opengl-sdk
..\premake\premake4.exe vs2010
[open and build solution]
```

### Building the application

On Linux :

```bash
premake/premake4 gmake
make
```

On Windows :

```bash
premake\premake4.exe vs2010
[open and build solution]
```
