@echo "Generating project..."
premake\premake4 vs2010
@echo "[open and build solution]"

@echo "==== Running cpplint ===="
python cpplint.py --verbose=3 --output=eclipse --linelength=120 src/Main/Main.cpp src/Main/App.h src/Main/App.cpp src/Main/Input.h src/Main/Input.cpp src/Main/Renderer.h src/Main/Renderer.cpp  src/Main/Node.h src/Main/Node.cpp

@echo "==== Running cppcheck ===="
cppcheck --quiet --enable=style --template=gcc src/

@echo "==== Running doxygen ===="
doxygen > NUL

