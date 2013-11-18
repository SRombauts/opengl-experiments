/**
 * @file    App.cpp
 * @ingroup opengl-experiments
 * @brief   Simple experiments in C++ using the Unofficial OpenGL SDK
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "App.h"        // NOLINT(build/include) no subdirectory (yet)

#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <fstream>      // NOLINT(readability/streams) for shader files
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

#include <cmath>    // cos, sin, tan

static const float X_LEFT   = -0.5f;    ///< Left coordinate
static const float X_RIGHT  = 0.5f;     ///< Right coordinate
static const float Y_TOP    = 0.5f;     ///< Top coordinate
static const float Y_BOTTOM = -0.5f;    ///< Bottom coordinate
static const float Z_FRONT  = -1.5f;    ///< Front coordinate
static const float Z_BACK   = -2.5f;    ///< Back coordinate

/// Indices of vertex (data bellow)
static const GLshort _indexData[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1,
    6, 0, 4, 2,
    1, 7, 3, 5,
};
// static const int _nbIndices = sizeof(_indexData)/sizeof(_indexData[0]); ///< Number of indices
static const int _lenMainStrip          = 10;  ///< Number of indices for the main strip
static const int _offsetOfLeftStrip     = _lenMainStrip * sizeof(_indexData[0]);    ///< Offset of left strip
static const int _lenLeftStrip          = 4;   ///< Number of indices for the lateral left strip
static const int _offsetOfRightStrip    = (_lenMainStrip + _lenLeftStrip) * sizeof(_indexData[0]); ///< Offset of right strip
static const int _lenRightStrip         = 4;  ///< Number of indices for the lateral right strip

/// Vertex data (indexed above), followed by their color data
///   6 - 7
///  /   /|
/// 0 - 1 5
/// |   |/
/// 2 - 3
///
/// 0, 1, 2, 3,
/// 4, 5, 6, 7,
/// 0, 1
static const float _vertexData[] = {
    // the 8 vertices (x,y,z,w) but w default to 1.0f
    X_LEFT,  Y_TOP,     Z_FRONT,
    X_RIGHT, Y_TOP,     Z_FRONT,
    X_LEFT,  Y_BOTTOM,  Z_FRONT,
    X_RIGHT, Y_BOTTOM,  Z_FRONT,
    X_LEFT,  Y_BOTTOM,  Z_BACK,
    X_RIGHT, Y_BOTTOM,  Z_BACK,
    X_LEFT,  Y_TOP,     Z_BACK,
    X_RIGHT, Y_TOP,     Z_BACK,
    // the colors (r,g,b,a) of each of 8 vertices
    0.8f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.8f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.8f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,
    0.0f, 0.5f, 0.5f, 1.0f,
    0.5f, 0.0f, 0.5f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f,
    0.1f, 0.1f, 0.1f, 1.0f
};
static const int _nbVertices        = 8;    ///< Number of vertices
static const int _vertexDim         = 3;    ///< Each vertex is in 3D (x,y,z) (so w default to 1.0f)
static const int _sizeOfVertex      = _vertexDim * sizeof(float);   ///< size of one vertex
static const int _offsetOfColors    = _nbVertices * _sizeOfVertex;  ///< size of all vertices == start of colors
static const int _colorDim          = 4;    ///< Each color is in 4D (r,g,b,a)


// Definition of the static pointer to the unique App instance
App* App::_mpSelf = NULL;


/**
 * @brief Calculation of the frustum scale factor for a target Field of View
 *
 * @param[in] aFovDeg   Field of View in degree
 *
 * @return Frustum scale
*/
float CalcFrustumScale(float aFovDeg) {
    const float degToRad = 3.14159f * 2.0f / 360.0f;
    float fovRad = aFovDeg * degToRad;
    return 1.0f / tan(fovRad / 2.0f);
}
static const float _frustumScale    = CalcFrustumScale(45.0f);  ///< frustum scale factor for a target Field of View
static const float _zNear           = 1.0f;     ///< Z coordinate or the near/front frustum plane from which to render
static const float _zFar            = 100.0f;   ///< Z coordinate or the far/back frustum plane to which to render


/**
 * @brief Constructor
 */
App::App() :
    mLog("App"),
    mProgram(0),
    mPositionAttrib(-1),
    mColorAttrib(-1),
    mModelToWorldMatrixUnif(-1),
    mWorldToCameraMatrixUnif(-1),
    mCameraToClipMatrixUnif(-1),
    mVertexBufferObject(0),
    mIndexBufferObject(0),
    mVertexArrayObject(0),
    mModelRotation(0.0f),
    mModelTranslation(0.0f),
    mModelToWorldMatrix(1.0f),
    mCameraToClipMatrix(1.0f) {
    _mpSelf = this;
    init();
}
/**
 * @brief Destructor
 */
App::~App() {
    uninitVertexArrayObject();
    _mpSelf = NULL;
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
void App::compileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const {
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
void App::init() {
    // 1) compile shaders and link them in a program
    initProgram();

    // 2) init the vertex buffer and vertex array objects
    initVertexArrayObject();

    // 3) Register GLUT Callbacks
    registerCallbacks();

    // 4) Initialize more OpenGL option
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
void App::initProgram() {
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
    mModelToWorldMatrixUnif     = glGetUniformLocation(mProgram, "modelToWorldMatrix");
    mWorldToCameraMatrixUnif    = glGetUniformLocation(mProgram, "worldToCameraMatrix");
    mCameraToClipMatrixUnif     = glGetUniformLocation(mProgram, "cameraToClipMatrix");

    // Define the "Camera to Clip" matrix for the perspective transformation
    mCameraToClipMatrix[0].x = _frustumScale;
    mCameraToClipMatrix[1].y = _frustumScale;
    mCameraToClipMatrix[2].z = (_zFar + _zNear) / (_zNear - _zFar);
    mCameraToClipMatrix[2].w = -1.0f;
    mCameraToClipMatrix[3].z = (2 * _zFar * _zNear) / (_zNear - _zFar);

    // Set uniform values for matrix transformations
    glUseProgram(mProgram);
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(mModelToWorldMatrix));
    glUniformMatrix4fv(mWorldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // unity matrix
//  glUniformMatrix4fv(mCameraToClipMatrixUnif,  1, GL_FALSE, glm::value_ptr(mCameraToClipMatrix)); // reshape()
    glUseProgram(0);
}

/**
 * @brief Initialize the vertex buffer and vertex array objects
 *
 *  Init the vertex buffer object with the data of our mesh (triangle strip)
 * and bind it to a vertex array to 
 */
void App::initVertexArrayObject(void) {
    // Generate a VBO: Ask for a buffer of GPU memory
    mLog.debug() << "initializing vertex buffer objet...";
    // TODO(SRombauts) test buffers != 0
    glGenBuffers(1, &mVertexBufferObject);

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_vertexData), _vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // here _vertexData is of no more use (dynamic memory could be deallocated)

    // Generate a IBO: Ask for a buffer of GPU memory
    mLog.debug() << "initializing index buffer objet...";
    glGenBuffers(1, &mIndexBufferObject);

    // Allocate GPU memory and copy our data onto this new buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indexData), _indexData, GL_STATIC_DRAW);
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
void App::uninitVertexArrayObject(void) {
    mLog.debug() << "uninitializing vertex buffer and array objet...";
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

/**
 * @brief Register GLUT callbacks
 */
void App::registerCallbacks() {
    mLog.debug() << "registerCallbacks...";
    glutReshapeFunc(reshapeCallbackStatic);
    glutDisplayFunc(displayCallbackStatic);
    glutKeyboardFunc(keyboardCallbackStatic);
    glutSpecialFunc(keyboardSpecialCallbackStatic);
    glutMouseFunc(mouseCallbackStatic);
    glutMotionFunc(mouseMotionCallbackStatic);
    glutPassiveMotionFunc(mousePassiveMotionCallbackStatic);
    glutMouseWheelFunc(mouseWheelCallbackStatic);
    glutJoystickFunc(joystickCallbackStatic, 10);
}

/**
 * @brief Move up the model
 */
void App::up() {
    mModelTranslation.y += 0.01f;
    mModelToWorldMatrix[3].y = mModelTranslation.y;

    mLog.info() << "up y=" << mModelTranslation.y;
}
/**
 * @brief Move down the model
 */
void App::down() {
    mModelTranslation.y -= 0.01f;
    mModelToWorldMatrix[3].y = mModelTranslation.y;

    mLog.info() << "down y=" << mModelTranslation.y;
}

/**
 * @brief Move the model to the left
 */
void App::left() {
    mModelTranslation.x -= 0.01f;
    mModelToWorldMatrix[3].x = mModelTranslation.x;

    mLog.info() << "left: x=" << mModelTranslation.x;
}
/**
 * @brief Move the model to the right
 */
void App::right() {
    mModelTranslation.x += 0.01f;
    mModelToWorldMatrix[3].x = mModelTranslation.x;

    mLog.info() << "right: x=" << mModelTranslation.x;
}

/**
 * @brief Move the model to the front
 */
void App::front() {
    mModelTranslation.z += 0.01f;
    mModelToWorldMatrix[3].z = mModelTranslation.z;

    mLog.info() << "front: z=" << mModelTranslation.z;
}
/**
 * @brief Move the model to the back
 */
void App::back() {
    mModelTranslation.z -= 0.01f;
    mModelToWorldMatrix[3].z = mModelTranslation.z;

    mLog.info() << "back: z=" << mModelTranslation.z;
}

/**
 * @brief Rotate the model
 */
void App::rotate(int aDeltaX, int aDeltaY) {
    mModelRotation.x += (aDeltaX * 0.01f);
    mModelRotation.y += (aDeltaY * 0.01f);

    // Rotation arround the Y axis (from left to right)
    mModelToWorldMatrix[0].x = cos(mModelRotation.x);
    mModelToWorldMatrix[0].z = -sin(mModelRotation.x);
    mModelToWorldMatrix[2].x = sin(mModelRotation.x);
    mModelToWorldMatrix[2].z = cos(mModelRotation.x);

    // Rotation arround the X axis (from top to bottom)
    mModelToWorldMatrix[1].y = cos(mModelRotation.y);
    mModelToWorldMatrix[1].z = sin(mModelRotation.y);
    mModelToWorldMatrix[2].y = -sin(mModelRotation.y);
    mModelToWorldMatrix[2].z = cos(mModelRotation.y);

    mLog.info() << "rotate: angle(" << mModelRotation.x << ", " << mModelRotation.y << ")";
}

/**
 * @brief GLUT reshape callback function
 *
 *  Called once at the start of the rendering, and then for each window resising.
 *
 * @param[in] aW    Largeur utile de la fenêtre
 * @param[in] aH    Hauteur utile de la fenêtre
 */
void App::reshapeCallback(int aW, int aH) {
    mLog.info() << "reshapeCallback(" << aW << "," << aH << ")";

    // Calculate ratio with new window ratio
    mCameraToClipMatrix[0].x = _frustumScale * static_cast<float>(aH)/static_cast<float>(aW);
    mCameraToClipMatrix[1].y = _frustumScale;

    // Set uniform values with the new matrix transformation
    glUseProgram(mProgram);
    glUniformMatrix4fv(mCameraToClipMatrixUnif,  1, GL_FALSE, glm::value_ptr(mCameraToClipMatrix));
    glUseProgram(0);

    glViewport(0, 0, (GLsizei)aW, (GLsizei)aH);
}

/**
 * @brief GLUT display callback function
 */
void App::displayCallback() {
    // mLog.debug() << "displayCallback()";

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(mProgram);

    // Bind the Vertex Array Object, bound to buffers with vertex position and colors
    glBindVertexArray(mVertexArrayObject);

    // Ask to Draw triangles from buffers pointed by the Vertex Array Object
    glDrawElements(GL_TRIANGLE_STRIP, _lenMainStrip,  GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLE_STRIP, _lenLeftStrip,  GL_UNSIGNED_SHORT, reinterpret_cast<void*>(_offsetOfLeftStrip));
    glDrawElements(GL_TRIANGLE_STRIP, _lenRightStrip, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(_offsetOfRightStrip));

    // Unbind the Vertex Array Object
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();    // Ask for refresh ; only needed if animation are present
}

/**
 * @brief GLUT keyboard callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void App::keyboardCallback(unsigned char aKey, int aX, int aY) {
    mLog.debug() << "keyboardCallback(" << static_cast<int>(aKey) << "='" << aKey
                        << "'," << aX << "," << aY << ")";
    switch (aKey) {
        case 27:   // ESC : EXIT
            glutLeaveMainLoop();
            break;
        case 'w': case 'W':  // QWERTY keyboard disposition
        case 'z': case 'Z':  // AZERTY keyboard disposition
            up();
            break;
        case 'a': case 'A':  // QWERTY
        case 'q': case 'Q':  // AZERTY
            left();
            break;
        case 's': case 'S':  // QWERTY & AZERTY
            down();
            break;
        case 'd': case 'D':  // QWERTY & AZERTY
            right();
            break;
        default:
            break;
    }

    // Set uniform values with the new "Model to World" matrix
    glUseProgram(mProgram);
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(mModelToWorldMatrix));
    glUseProgram(0);
}

/**
 * @brief GLUT special keys keyboard callback function
 *
 * @param[in] aKey  ASCII code of the key pressed
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void App::keyboardSpecialCallback(int aKey, int aX, int aY) {
    mLog.info() << "keyboardCallback(" << aKey << "," << aX << "," << aY << ")";

    switch (aKey) {
        case GLUT_KEY_UP:
            up();
            break;
        case GLUT_KEY_LEFT:
            left();
            break;
        case GLUT_KEY_DOWN:
            down();
            break;
        case GLUT_KEY_RIGHT:
            right();
            break;
        default:
            break;
    }

    // Set uniform values with the new "Model to World" matrix
    glUseProgram(mProgram);
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(mModelToWorldMatrix));
    glUseProgram(0);
}

/**
 * @brief GLUT mouse button click callback function
 *
 * @param[in] aButton   Mouse button
 * @param[in] aState    State of the button
 * @param[in] aX        X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY        Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void App::mouseCallback(int aButton, int aState, int aX, int aY) {
    mLog.info() << "mouseCallback(" << aButton << "," << ((aState == GLUT_DOWN)?"down":"up")
              << "," << aX << "," << aY << ")";
}


static glm::detail::tvec2<int> _lastMousePos;   ///< Last known position of the mouse for mouse (passive) motion functions

/**
 * @brief GLUT mouse motion (while button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */    
void App::mouseMotionCallback(int aX, int aY) {
    mLog.info() << "mouseMotionCallback(" << aX << "," << aY << ")";

    if (   (aX != _lastMousePos.x)
        || (aY != _lastMousePos.y) ) {
        rotate((aX - _lastMousePos.x), (aY - _lastMousePos.y));
        _lastMousePos.x = aX;
        _lastMousePos.y = aY;

        // Set uniform values with the new "Model to World" matrix
        glUseProgram(mProgram);
        glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(mModelToWorldMatrix));
        glUseProgram(0);
    }
}

/**
 * @brief GLUT mouse passive motion (while no button pressed) callback function
 *
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */    
void App::mousePassiveMotionCallback(int aX, int aY) {
    mLog.info() << "mousePassiveMotionCallback(" << aX << "," << aY << ")";

    _lastMousePos.x = aX;
    _lastMousePos.y = aY;
}

/**
 * @brief GLUT mouse wheel callback function (LOW SENSITIVITY under Windows)
 *
 * @param[in] aNum  Wheel number (0)
 * @param[in] aDirection Direction 1=up, or -1=down
 * @param[in] aX    X coord of the mouse cursor (0 is the left of the render surface of the window: can be negative!)
 * @param[in] aY    Y coord of the mouse cursor (0 is the top of the render surface of the window: can be negative!)
 */
void App::mouseWheelCallback(int aNum, int aDirection, int aX, int aY) {
    mLog.info() << "mouseWheelCallback(" << aNum << "," << aDirection << "," << aX << "," << aY << ")";

    if (0 < aDirection) {
        front();
    } else {
        back();
    }

    // Set uniform values with the new "Model to World" matrix
    glUseProgram(mProgram);
    glUniformMatrix4fv(mModelToWorldMatrixUnif,  1, GL_FALSE, glm::value_ptr(mModelToWorldMatrix));
    glUseProgram(0);
}

/**
 * @brief GLUT joystick callback function
 *
 * @param[in] aButtonMask Mask of buttons
 * @param[in] aX    X coord of the joystick (between -1000 and 1000)
 * @param[in] aY    Y coord of the joystick (between -1000 and 1000)
 * @param[in] aZ    Z coord of the joystick (between -1000 and 1000)
 */
void App::joystickCallback(unsigned int aButtonMask, int aX, int aY, int aZ) {
    static unsigned int             _lastButtonMask = 0;
    static glm::detail::tvec3<int>  _lastJoystickPos;
    if (   (_lastButtonMask != aButtonMask)
        || (_lastJoystickPos.x != aX) || (_lastJoystickPos.y != aY) || (_lastJoystickPos.z != aZ) ) {
        mLog.info() << "joystickCallback(" << aButtonMask << "," << aX << "," << aY << "," << aZ << ")";
        _lastButtonMask = aButtonMask;
        _lastJoystickPos.x = aX;
        _lastJoystickPos.y = aY;
        _lastJoystickPos.z = aZ;
    }
}
