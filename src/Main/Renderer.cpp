/**
 * @file    Renderer.cpp
 * @ingroup opengl-experiments
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Renderer.h"

#include "Utils/Time.h"

#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::perspective, glm::rotate, glm::translate

#include <fstream>      // NOLINT(readability/streams) for shader files
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include <cmath>    // cos, sin, tan

// Cube coordinates
static const float X_LEFT   = -0.5f;    ///< Left coordinate
static const float X_RIGHT  = 0.5f;     ///< Right coordinate
static const float Y_TOP    = 0.5f;     ///< Top coordinate
static const float Y_BOTTOM = -0.5f;    ///< Bottom coordinate
static const float Z_FRONT  = 0.5f;    ///< Front coordinate
static const float Z_BACK   = -0.5f;    ///< Back coordinate
// Plane coordinates
static const float X_PLANE_LEFT     = -5.0f;    ///< Left coordinate
static const float X_PLANE_RIGHT    = 5.0f;     ///< Right coordinate
static const float Y_PLANE          = -0.5f;    ///< Y coordinate
static const float Z_PLANE_FRONT    = 5.0f;     ///< Front coordinate
static const float Z_PLANE_BACK     = -5.0f;    ///< Back coordinate

/// Vertex data (indexed bellow), followed by their color data
/// cube:
///   6 - 7
///  /   /|
/// 0 - 1 5
/// |   |/
/// 2 - 3
/// plane:
///   0 - 1
///  /   /
/// 2 - 3
static const float _vertexData[] = {
    // cube: the 8 vertices (x,y,z,w) but w default to 1.0f
    X_LEFT,  Y_TOP,     Z_FRONT,
    X_RIGHT, Y_TOP,     Z_FRONT,
    X_LEFT,  Y_BOTTOM,  Z_FRONT,
    X_RIGHT, Y_BOTTOM,  Z_FRONT,
    X_LEFT,  Y_BOTTOM,  Z_BACK,
    X_RIGHT, Y_BOTTOM,  Z_BACK,
    X_LEFT,  Y_TOP,     Z_BACK,
    X_RIGHT, Y_TOP,     Z_BACK,
    // plane: the 4 vertices (x,y,z,w) but w default to 1.0f
    X_PLANE_LEFT,  Y_PLANE, Z_PLANE_BACK,
    X_PLANE_RIGHT, Y_PLANE, Z_PLANE_BACK,
    X_PLANE_LEFT,  Y_PLANE, Z_PLANE_FRONT,
    X_PLANE_RIGHT, Y_PLANE, Z_PLANE_FRONT,
    // cube: the colors (r,g,b,a) of each of 8 vertices
    0.8f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.8f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.8f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,
    0.0f, 0.5f, 0.5f, 1.0f,
    0.5f, 0.0f, 0.5f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f,
    0.1f, 0.1f, 0.1f, 1.0f,
    // plane: the colors (r,g,b,a) of each of 4 vertices
    0.1f, 0.8f, 0.3f, 1.0f,
    0.1f, 0.8f, 0.3f, 1.0f,
    0.1f, 0.8f, 0.3f, 1.0f,
    0.1f, 0.8f, 0.3f, 1.0f,
};
static const int _sizeOfVertexData  = sizeof(_vertexData);  ///< size of the _vertexData array
static const int _nbVertices        = 12;                   ///< Total number of vertices (cube + plane)
static const int _vertexDim         = 3;                    ///< Each vertex is in 3D (x,y,z) (so w default to 1.0f)
static const int _sizeOfVertex      = _vertexDim * sizeof(_vertexData[0]);  ///< size of one vertex
static const int _offsetOfColors    = _nbVertices * _sizeOfVertex;          ///< size of all vertices == start of colors
static const int _colorDim          = 4;                    ///< Each color is in 4D (r,g,b,a)

/// Indices of vertex (from vertex buffer above)
static const GLshort _indexData[] = {
    // cube:
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1,
    6, 0, 4, 2,
    1, 7, 3, 5,
    // plane
    8, 9, 10, 11,
};
static const int _sizeOfIndexData       = sizeof(_indexData);   ///< size of the _indexData);  ///< size array
static const int _lenMainStrip          = 10;                   ///< Number of indices for the main strip
/// Offset of the lateral strip at the left of the cube
static const int _offsetOfLeftStrip     = _lenMainStrip * sizeof(_indexData[0]);
static const int _lenLeftStrip          = 4;                    ///< Number of indices for the lateral left strip
/// Offset of the lateral strip at the right of the cube :
static const int _offsetOfRightStrip    = (_lenMainStrip + _lenLeftStrip) * sizeof(_indexData[0]);
static const int _lenRightStrip         = 4;                    ///< Number of indices for the lateral right strip
/// Offset of the plane strip :
static const int _offsetOfPlaneStrip    = (_lenMainStrip + _lenLeftStrip + _lenRightStrip) * sizeof(_indexData[0]);
static const int _lenPlaneStrip         = 4;                    ///< Number of indices for the plane strip


static const float _zNear           = 1.0f;     ///< Z coordinate or the near/front frustum plane from which to render
static const float _zFar            = 100.0f;   ///< Z coordinate or the far/back frustum plane to which to render


/**
 * @brief Constructor
 */
Renderer::Renderer() :
    mLog("Renderer"),
    mProgram(0),
    mPositionAttrib(-1),
    mColorAttrib(-1),
    mModelToWorldMatrixUnif(-1),
    mWorldToCameraMatrixUnif(-1),
    mCameraToClipMatrixUnif(-1),
    mVertexBufferObject(0),
    mIndexBufferObject(0),
    mVertexArrayObject(0),
    mCameraRotation(0.0f),
    mCameraTranslation(0.0f, 0.0f, -5.0f),
    mModelRotation(0.0f),
    mModelTranslation(1.0f, 0.0f, 0.0f) {
    init();
}
/**
 * @brief Destructor
 */
Renderer::~Renderer() {
    uninitVertexArrayObject();
}


/**
 * @brief Compile a given type shader from the content of a file, and add it to the list
 *
 * @param[in,out]   aShaderList         List of compiled shaders, to be completed by this function
 * @param[in]       aShaderType         Type of shader to be compiled
 * @param[in]       apShaderFilename    Name of the shader file to compile
 *
 * @throw a std::exception in case of error (glutil::CompileLinkException or std::runtime_error)
 */
void Renderer::compileShader(std::vector<GLuint>& aShaderList,
                        const GLenum aShaderType,
                        const char* apShaderFilename) const {
    try {
        std::ifstream ifShader(apShaderFilename);
        if (ifShader.is_open()) {
            std::ostringstream isShader;
            isShader << ifShader.rdbuf();
            mLog.debug() << "compileShader: compiling \"" << apShaderFilename << "\"...";
            aShaderList.push_back(glutil::CompileShader(aShaderType, isShader.str()));
        } else {
            mLog.critic() << "compileShader: unavailable file \"" << apShaderFilename << "\"";
            throw std::runtime_error(std::string("compileShader: unavailable file ") + apShaderFilename);
        }
    }
    catch(glutil::CompileLinkException& e) {
        mLog.info() << "compileShader: \"" << apShaderFilename << "\":\n" << e.what();
        throw;  // rethrow to abort program
    }
}

/**
 * @brief Initialization
 */
void Renderer::init() {
    // 1) compile shaders and link them in a program
    initProgram();

    // 2) init the vertex buffer and vertex array objects
    initVertexArrayObject();

    // 3) Initialize more OpenGL option
    // Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}

/**
 * @brief compile shaders and link them in a program
 */
void Renderer::initProgram() {
    std::vector<GLuint> shaderList;

    // Compile the shader files (into intermediate compiled object)
    mLog.debug() << "initProgram: compiling shaders...";
    compileShader(shaderList, GL_VERTEX_SHADER,   "data/ModelWorldCameraClip.vert");
    compileShader(shaderList, GL_FRAGMENT_SHADER, "data/PassthroughColor.frag");
    // Link them in a program (into the final executable to send to the GPU)
    mLog.debug() << "initProgram: linking program...";
    mProgram = glutil::LinkProgram(shaderList);
    // Now, the intermediate compiled shader can be deleted (the program contain them)
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    // Get location of (vertex) attributes (input streams of (vertex) shader
    // TODO(SRombauts) test attribute and uniform != -1
    mPositionAttrib = glGetAttribLocation(mProgram, "position");   // layout(location = 0) in vec4 position;
    mColorAttrib    = glGetAttribLocation(mProgram, "color");      // layout(location = 1) in vec4 color;
    // Get location of uniforms - input variables of (vertex) shader
    // "Model to World" matrix,  positioning our unique model into world space
    // "World to Camera" matrix, defining the orientation of the viewer
    // "Camera to Clip" matrix,  defining the perspective transformation
    mModelToWorldMatrixUnif     = glGetUniformLocation(mProgram, "modelToWorldMatrix");
    mWorldToCameraMatrixUnif    = glGetUniformLocation(mProgram, "worldToCameraMatrix");
    mCameraToClipMatrixUnif     = glGetUniformLocation(mProgram, "cameraToClipMatrix");
}

/**
 * @brief Initialize the vertex buffer and vertex array objects
 *
 *  Init the vertex buffer object with the data of our mesh (triangle strip)
 * and bind it to a vertex array to 
 */
void Renderer::initVertexArrayObject(void) {
    // Generate a VBO: Ask for a buffer of GPU memory
    mLog.debug() << "initializing vertex buffer objet...";
    // TODO(SRombauts) test buffers != 0
    glGenBuffers(1, &mVertexBufferObject);

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, _sizeOfVertexData, _vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // here _vertexData is of no more use (dynamic memory could be deallocated)

    // Generate a IBO: Ask for a buffer of GPU memory
    mLog.debug() << "initializing index buffer objet...";
    glGenBuffers(1, &mIndexBufferObject);

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeOfIndexData, _indexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // here _indexData is of no more use (dynamic memory could be deallocated)

    // Generate a VAO: Ask for a place on GPU to associate states with our data
    mLog.debug() << "initializing vertex array objet...";
    glGenVertexArrays(1, &mVertexArrayObject);

    // Bind the vertex array, so that it can memorize the following states
    glBindVertexArray(mVertexArrayObject);

    // Bind the vertex buffer, and init vertex position and colors input streams (shader attributes)
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glEnableVertexAttribArray(mPositionAttrib);    // layout(location = 0) in vec4 position;
    glEnableVertexAttribArray(mColorAttrib);       // layout(location = 1) in vec4 color;
    // this tells the GPU witch part of the buffer to route to which attribute (shader input stream)
    glVertexAttribPointer(mPositionAttrib, _vertexDim, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(mColorAttrib, _colorDim, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(_offsetOfColors));
    // this tells OpenGL that vertex are pointed by their index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

    glBindVertexArray(0);
}

/**
 * @brief Uninitialize the vertex buffer and vertex array objects
 */
void Renderer::uninitVertexArrayObject(void) {
    mLog.debug() << "uninitializing vertex buffer and array objet...";
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

/**
 * @brief Move up the camera
 */
void Renderer::up() {
    mCameraTranslation.y += 0.01f;
    mLog.info() << "up y=" << mCameraTranslation.y;
}
/**
 * @brief Move down the camera
 */
void Renderer::down() {
    mCameraTranslation.y -= 0.01f;
    mLog.info() << "down y=" << mCameraTranslation.y;
}

/**
 * @brief Move the camera to the left
 */
void Renderer::left() {
    mCameraTranslation.x -= 0.01f;
    mLog.info() << "left: x=" << mCameraTranslation.x;
}
/**
 * @brief Move the camera to the right
 */
void Renderer::right() {
    mCameraTranslation.x += 0.01f;
    mLog.info() << "right: x=" << mCameraTranslation.x;
}

/**
 * @brief Move the camera to the front
 */
void Renderer::front() {
    mCameraTranslation.z += 0.1f;
    mLog.info() << "front: z=" << mCameraTranslation.z;
}
/**
 * @brief Move the camera to the back
 */
void Renderer::back() {
    mCameraTranslation.z -= 0.1f;
    mLog.info() << "back: z=" << mCameraTranslation.z;
}

/**
 * @brief Rotate the camera
 */
void Renderer::rotate(int aDeltaX, int aDeltaY) {
    mCameraRotation.x += (aDeltaX * 0.01f);
    mCameraRotation.y += (aDeltaY * 0.01f);
    mLog.info() << "rotate: angle(" << mCameraRotation.x << ", " << mCameraRotation.y << ")";
}

/**
 * @brief Calculate the new camera transformation matrix from Rotations and Translations
 */
glm::mat4 Renderer::transform() {
    // Rotation around the Y axis (from left to right)
    glm::mat4 rotationY;
    rotationY[0].x = cos(mCameraRotation.x);
    rotationY[0].z = -sin(mCameraRotation.x);
    rotationY[2].x = sin(mCameraRotation.x);
    rotationY[2].z = cos(mCameraRotation.x);

    // Rotation around the X axis (from top to bottom)
    glm::mat4 rotationX;
    rotationX[1].y = cos(mCameraRotation.y);
    rotationX[1].z = sin(mCameraRotation.y);
    rotationX[2].y = -sin(mCameraRotation.y);
    rotationX[2].z = cos(mCameraRotation.y);

    // Translation at last
    glm::mat4 translations = glm::translate(glm::mat4(1.0f), mCameraTranslation);

    // Calculate the new "worldToCameradMatrix" (from right to left)
    return (translations * rotationX * rotationY);
}


/**
 * @brief Move up the model
 */
void Renderer::modelUp() {
    mModelTranslation.y += 0.01f;
    mLog.info() << "model up y=" << mModelTranslation.y;
}
/**
 * @brief Move down the model
 */
void Renderer::modelDown() {
    mModelTranslation.y -= 0.01f;
    mLog.info() << "model down y=" << mModelTranslation.y;
}

/**
 * @brief Move the model to the left
 */
void Renderer::modelLeft() {
    mModelTranslation.x -= 0.01f;
    mLog.info() << "model left: x=" << mModelTranslation.x;
}
/**
 * @brief Move the model to the right
 */
void Renderer::modelRight() {
    mModelTranslation.x += 0.01f;
    mLog.info() << "model right: x=" << mModelTranslation.x;
}

/**
 * @brief Move the model to the front
 */
void Renderer::modelFront() {
    mModelTranslation.z += 0.01f;
    mLog.info() << "model front: z=" << mModelTranslation.z;
}
/**
 * @brief Move the model to the back
 */
void Renderer::modelBack() {
    mModelTranslation.z -= 0.01f;
    mLog.info() << "model back: z=" << mModelTranslation.z;
}

/**
 * @brief Rotate the model
 */
void Renderer::modelRotate(int aDeltaX, int aDeltaY) {
    mModelRotation.x += (aDeltaX * 0.01f);
    mModelRotation.y += (aDeltaY * 0.01f);
    //mLog.info() << "model rotate: angle(" << mModelRotation.x << ", " << mModelRotation.y << ")";
}

/**
 * @brief Calculate the new model transformation matrix from Rotations and Translations
 */
glm::mat4 Renderer::modelTransform() {
    // Rotation around the Y axis (from left to right)
    glm::mat4 rotationY;
    rotationY[0].x = cos(mModelRotation.x);
    rotationY[0].z = -sin(mModelRotation.x);
    rotationY[2].x = sin(mModelRotation.x);
    rotationY[2].z = cos(mModelRotation.x);

    // Rotation around the X axis (from top to bottom)
    glm::mat4 rotationX;
    rotationX[1].y = cos(mModelRotation.y);
    rotationX[1].z = sin(mModelRotation.y);
    rotationX[2].y = -sin(mModelRotation.y);
    rotationX[2].z = cos(mModelRotation.y);

    // Translations at last
    glm::mat4 translations = glm::translate(glm::mat4(1.0f), mModelTranslation);

    // Calculate the new "modelToWorldMatrix" (from right to left)
    return (translations * rotationX * rotationY);
}

/**
 * @brief Calculate and print FPS, and average and word frame duration
 */
void Renderer::calculateFPS() {
    static int      _nbFrames       = 0;
    static time_t   _worstFrameUs   = 0;
    static time_t   _firstTickUs    = Utils::Time::getTickUs();
    static time_t   _lastTickUs     = Utils::Time::getTickUs();
    time_t          curTickUs       = Utils::Time::getTickUs();
    time_t          totalUs         = (curTickUs - _firstTickUs);
    time_t          frameUs         = (curTickUs - _lastTickUs);

    _lastTickUs = curTickUs;
    ++_nbFrames;

    if (frameUs > _worstFrameUs) {
        _worstFrameUs = frameUs;
    }
    if (totalUs >= 1000000) {
        time_t avgUs = totalUs/_nbFrames;
        mLog.notice() << _nbFrames << "fps (avg " << avgUs/1000 << "." << avgUs%1000
                      << "ms, worst " << _worstFrameUs/1000 << "." << _worstFrameUs%1000 << "ms)";
        _firstTickUs =  Utils::Time::getTickUs(); // do not count logging time
        _nbFrames = 0;
        _worstFrameUs = 0;
    }
}

/**
 * @brief GLUT reshape callback function
 *
 *  Called once at the start of the rendering, and then for each window resizing.
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void Renderer::reshape(int aW, int aH) {
    mLog.info() << "reshapeCallback(" << aW << "," << aH << ")";

    // Define the "Camera to Clip" matrix for the perspective transformation
    glm::mat4 cameraToClipMatrix = glm::perspective<float>(45.0f, (static_cast<float>(aW) / static_cast<float>(aH)), _zNear, _zFar);

    // Set uniform values with the new "Camera to Clip" matrix
    glUseProgram(mProgram);
    glUniformMatrix4fv(mCameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
    glUseProgram(0);

    glViewport(0, 0, (GLsizei)aW, (GLsizei)aH);
}

/**
 * @brief GLUT display callback function
 */
void Renderer::display() {
    // mLog.debug() << "displayCallback()";

    // FPS and frame duration calculations
    calculateFPS();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(mProgram);

    // re-calculate the "World to Camera" matrix, and pass it to the program
    glm::mat4 worldToCamerMatrix = transform();
    // Set uniform values with the new "World to Camera" matrix
    glUniformMatrix4fv(mWorldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(worldToCamerMatrix));

    // Bind the Vertex Array Object, bound to buffers with vertex position and colors
    glBindVertexArray(mVertexArrayObject);

    // Use a matrix stack to manage the tree-hierarchy of the scene (initialized with the identity matrix)
    glutil::MatrixStack modelToWorldMatrixStack;
    drawPlane(modelToWorldMatrixStack);

    // Unbind the Vertex Array Object
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();    // Ask for refresh ; only needed if animation are present
}

/**
 * @brief Draw the plane at the root of the world
 *
 * @param[in] aModelToWorldMatrixStack  Matrix Stack of the tree of the scene
 */
void Renderer::drawPlane(glutil::MatrixStack& aModelToWorldMatrixStack) {
    // Root of the stack : the plane is centered at the origin, unscaled (no transformation, need to push the stack)

    // Set uniform values with the new "Model to World" matrix
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(aModelToWorldMatrixStack.Top()));
    // Ask to Draw triangles from buffers pointed by the Vertex Array Object
    // plane:
    glDrawElements(GL_TRIANGLE_STRIP, _lenPlaneStrip, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(_offsetOfPlaneStrip));

    // Now draw the sub elements
    drawCube(aModelToWorldMatrixStack);
}

/**
 * @brief Draw the cube above the plane
 *
 * First level of the stack : the cube is translated and rotated above the plane
 *
 * @param[in] aModelToWorldMatrixStack  Matrix Stack of the tree of the scene
 */
void Renderer::drawCube(glutil::MatrixStack& aModelToWorldMatrixStack) {
    glutil::PushStack push(aModelToWorldMatrixStack); // RAII PushStack

    // re-calculate the model to world transformations matrix, and pass it to the program
    glm::mat4 cubeToWorldMatrix = modelTransform();
    aModelToWorldMatrixStack.ApplyMatrix(cubeToWorldMatrix);

    // Set uniform values with the new "Model to World" matrix
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(aModelToWorldMatrixStack.Top()));
    // cube:
    glDrawElements(GL_TRIANGLE_STRIP, _lenMainStrip,  GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLE_STRIP, _lenLeftStrip,  GL_UNSIGNED_SHORT, reinterpret_cast<void*>(_offsetOfLeftStrip));
    glDrawElements(GL_TRIANGLE_STRIP, _lenRightStrip, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(_offsetOfRightStrip));
}
