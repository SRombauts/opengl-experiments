OpenGL Experiments in C++
-------------------------

### License

Copyright (c) 2012 Sébastien Rombauts (sebastien.rombauts@gmail.com)

Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
or copy at http://opensource.org/licenses/MIT)

### Building and Running

Complete the unofficial-opengl-sdk :

TODO not working (missing lua "ex" and "ufs" extensions)
#cd unofficial-opengl-sdk
#sudo apt-get install lua5.2 lua-socket liblua5.2-0
#lua get_externals.lua
Instead, download the unofficial-opengl-sdk package and copy the 4 missing libraries :
- unofficial-opengl-sdk/freeglut
- unofficial-opengl-sdk/glfw
- unofficial-opengl-sdk/glloadgen
- unofficial-opengl-sdk/glm

On Linux :

sudo apt-get install libxrandr-dev libxi-dev

cd unofficial-opengl-sdk
../premake/premake4 gmake
make
cd ..
premake/premake4 gmake
make

On Windows :

cd unofficial-opengl-sdk
..\premake\premake4 vs2010
[open and build solution]
cd ..
premake\premake4 vs2010
[open and build solution]
