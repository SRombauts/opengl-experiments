#!/bin/sh

echo "Generating project..."
premake/premake4 gmake
make -j4

echo "==== Running cpplint ===="
python cpplint.py --verbose=3 --output=eclipse --linelength=120 src/*/*

echo "==== Running cppcheck ===="
cppcheck --quiet --enable=style --template=gcc src/

echo "==== Running doxygen ===="
doxygen > /dev/null

