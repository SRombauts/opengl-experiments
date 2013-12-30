@echo "Generating project..."
premake\premake4 vs2010
@echo "[open and build solution]"

@echo "==== Running cpplint ===="
python cpplint.py --verbose=3 --output=vs7 --linelength=120 src/Main/App.h src/Main/App.cpp src/Main/Input.h src/Main/Input.cpp src/Main/Main.cpp src/Main/MatrixStack.h src/Main/Mesh.h src/Main/Mesh.cpp src/Main/Node.h src/Main/Node.cpp src/Main/Physic.h src/Main/Physic.cpp src/Main/Renderer.h src/Main/Renderer.cpp src/Main/Scene.h src/Main/Scene.cpp src/Main/ShaderProgram.h src/Main/ShaderProgram.cpp

@echo "==== Running cppcheck ===="
cppcheck --quiet --enable=style --template=vs src/

@echo "==== Running doxygen ===="
doxygen > NUL

