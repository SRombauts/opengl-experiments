OpenGL Experiments in C++
-------------------------

Basic experiments on OpenGL targeting the Oculus Rift Head Mounted Display (HMD) device.

### License

Copyright (c) 2012-2014 Sébastien Rombauts (sebastien.rombauts@gmail.com)

Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
or copy at http://opensource.org/licenses/MIT)

### Getting and Building the dependencies

1. Get the submodules (assimp, glfw, glload, glm, LoggerCpp, OculusSDK)

```bash
git submodule init
git submodule update
```

2. Get the development libraries under Linux :

```bash
sudo apt-get install libxrandr-dev libxi-dev libgl1-mesa-dev mesa-common-dev libudev-dev
```

### Building the application

On Linux :

```bash
cmake .
cmake --build .     # of simply "make"
```

On Windows :

```bash
cmake . -G "Visual Studio 10"
cmake --build .     # or simply [open and build solution]
```
