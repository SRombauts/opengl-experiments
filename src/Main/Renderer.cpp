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

#include <GL/freeglut.h>
#include <glutil/Shader.h>
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::perspective, glm::rotate, glm::translate

#include <assimp/cimport.h>     // Log Stream
#include <assimp/Importer.hpp>  // Open Asset Importer
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <fstream>      // NOLINT(readability/streams) for shader files
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include <cmath>    // cos, sin, tan

// We use a standard "Right Hand Coordinate System"
// Cube coordinates
static const float X_RIGHT  = 1.0f;     ///< Right coordinate
static const float X_LEFT   = -1.0f;    ///< Left coordinate
static const float Y_TOP    = 1.0f;     ///< Top coordinate
static const float Y_BOTTOM = -1.0f;    ///< Bottom coordinate
static const float Z_FRONT  = 1.0f;     ///< Front coordinate
static const float Z_BACK   = -1.0f;    ///< Back coordinate
// Plane coordinates
static const float X_PLANE_RIGHT    = 10.0f;    ///< Right coordinate
static const float X_PLANE_LEFT     = -10.0f;   ///< Left coordinate
static const float Y_PLANE          = 0.0f;     ///< Y coordinate
static const float Z_PLANE_FRONT    = 10.0f;    ///< Front coordinate
static const float Z_PLANE_BACK     = -10.0f;   ///< Back coordinate

/// Vertex data (indexed bellow), followed by their color data
/// We use the Clockwise winding order (GL_CW)
/// TODO SRO this is the inverse of OpenGL default CCW winding order
/// cube (6 faces with normals):
///                           12- 13
///                          /   /
///                 10- 11  14- 15      16     21
///                 | \ |              /|     /|
/// 0 - 1    6 - 7  8 - 9            17 18  20 23
/// | / |   / \ /                     |/     |/
/// 2 - 3  4 - 5                      19     22
///
/// plane (1 face with normals):
///   0 - 1
///  /   /
/// 2 - 3
static const float _vertexData[] = {
    // cube: the 24 vertices (x,y,z,w) but w default to 1.0f
    // front
    X_LEFT,  Y_TOP,     Z_FRONT,    // 0
    X_RIGHT, Y_TOP,     Z_FRONT,    // 1
    X_LEFT,  Y_BOTTOM,  Z_FRONT,    // 2
    X_RIGHT, Y_BOTTOM,  Z_FRONT,    // 3
    // bottom
    X_LEFT,  Y_BOTTOM,  Z_FRONT,    // 4
    X_RIGHT, Y_BOTTOM,  Z_FRONT,    // 5
    X_LEFT,  Y_BOTTOM,  Z_BACK,     // 6
    X_RIGHT, Y_BOTTOM,  Z_BACK,     // 7
    // back
    X_LEFT,  Y_BOTTOM,  Z_BACK,     // 8
    X_RIGHT, Y_BOTTOM,  Z_BACK,     // 9
    X_LEFT,  Y_TOP,     Z_BACK,     // 10
    X_RIGHT, Y_TOP,     Z_BACK,     // 11
    // top
    X_LEFT,  Y_TOP,     Z_BACK,     // 12
    X_RIGHT, Y_TOP,     Z_BACK,     // 13
    X_LEFT,  Y_TOP,     Z_FRONT,    // 14
    X_RIGHT, Y_TOP,     Z_FRONT,    // 15
    // left
    X_LEFT,  Y_TOP,     Z_BACK,     // 16
    X_LEFT,  Y_TOP,     Z_FRONT,    // 17
    X_LEFT,  Y_BOTTOM,  Z_BACK,     // 18
    X_LEFT,  Y_BOTTOM,  Z_FRONT,    // 19
    // right
    X_RIGHT, Y_TOP,     Z_FRONT,    // 20
    X_RIGHT, Y_TOP,     Z_BACK,     // 21
    X_RIGHT, Y_BOTTOM,  Z_FRONT,    // 22
    X_RIGHT, Y_BOTTOM,  Z_BACK,     // 23

    // plane: the 4 vertices (x,y,z,w) but w default to 1.0f
    X_PLANE_LEFT,  Y_PLANE, Z_PLANE_BACK,
    X_PLANE_RIGHT, Y_PLANE, Z_PLANE_BACK,
    X_PLANE_LEFT,  Y_PLANE, Z_PLANE_FRONT,
    X_PLANE_RIGHT, Y_PLANE, Z_PLANE_FRONT,

    // cube: the colors (r,g,b,a) of each of 24 vertices
    // front
    0.7f, 0.0f, 0.0f, 1.0f,
    0.7f, 0.0f, 0.0f, 1.0f,
    0.7f, 0.0f, 0.0f, 1.0f,
    0.7f, 0.0f, 0.0f, 1.0f,
    // bottom
    0.0f, 0.7f, 0.0f, 1.0f,
    0.0f, 0.7f, 0.0f, 1.0f,
    0.0f, 0.7f, 0.0f, 1.0f,
    0.0f, 0.7f, 0.0f, 1.0f,
    // back
    0.0f, 0.0f, 0.7f, 1.0f,
    0.0f, 0.0f, 0.7f, 1.0f,
    0.0f, 0.0f, 0.7f, 1.0f,
    0.0f, 0.0f, 0.7f, 1.0f,
    // top
    0.5f, 0.4f, 0.0f, 1.0f,
    0.5f, 0.4f, 0.0f, 1.0f,
    0.5f, 0.4f, 0.0f, 1.0f,
    0.5f, 0.4f, 0.0f, 1.0f,
    // left
    0.0f, 0.5f, 0.4f, 1.0f,
    0.0f, 0.5f, 0.4f, 1.0f,
    0.0f, 0.5f, 0.4f, 1.0f,
    0.0f, 0.5f, 0.4f, 1.0f,
    // right
    0.4f, 0.0f, 0.5f, 1.0f,
    0.4f, 0.0f, 0.5f, 1.0f,
    0.4f, 0.0f, 0.5f, 1.0f,
    0.4f, 0.0f, 0.5f, 1.0f,

    // plane: the colors (r,g,b,a) of each of 4 vertices
    0.0f, 0.5f, 0.05f, 1.0f,
    0.0f, 0.5f, 0.05f, 1.0f,
    0.0f, 0.5f, 0.05f, 1.0f,
    0.0f, 0.5f, 0.05f, 1.0f,

    // cube, the normals
    // front
    0.0f,   0.0f,       Z_FRONT,
    0.0f,   0.0f,       Z_FRONT,
    0.0f,   0.0f,       Z_FRONT,
    0.0f,   0.0f,       Z_FRONT,
    // bottom
    0.0f,   Y_BOTTOM,   0.0f,
    0.0f,   Y_BOTTOM,   0.0f,
    0.0f,   Y_BOTTOM,   0.0f,
    0.0f,   Y_BOTTOM,   0.0f,
    // back
    0.0f,   0.0f,       Z_BACK,
    0.0f,   0.0f,       Z_BACK,
    0.0f,   0.0f,       Z_BACK,
    0.0f,   0.0f,       Z_BACK,
    // top
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
    // left
    X_LEFT, 0.0f,       0.0f,
    X_LEFT, 0.0f,       0.0f,
    X_LEFT, 0.0f,       0.0f,
    X_LEFT, 0.0f,       0.0f,
    // right
    X_RIGHT, 0.0f,      0.0f,
    X_RIGHT, 0.0f,      0.0f,
    X_RIGHT, 0.0f,      0.0f,
    X_RIGHT, 0.0f,      0.0f,

    // plane, the normals
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
    0.0f,   Y_TOP,      0.0f,
};
static const int _sizeOfDataArray   = sizeof(_vertexData);  ///< size of the _vertexData array
static const int _nbVertices        = 24 + 4;               ///< Total number of vertices (cube + plane)
static const int _vertexDim         = 3;                    ///< Each vertex is in 3D (x,y,z) (so w default to 1.0f)
static const int _sizeOfVertex      = _vertexDim * sizeof(_vertexData[0]);  ///< size of one vertex
static const int _offsetColors      = _nbVertices * _sizeOfVertex;          ///< size of vertices == start of colors
static const int _nbColors          = _nbVertices;          ///< Total number of colors (cube + plane)
static const int _colorDim          = 4;                    ///< Each color is in 4D (r,g,b,a)
static const int _sizeOfColor       = _colorDim * sizeof(_vertexData[0]);   ///< size of one color
static const int _offsetNormals     = _offsetColors + _nbColors * _sizeOfColor; ///< vertices + colors == start normals
static const int _nbNormals         = _nbVertices;          ///< Total number of colors (cube + plane)
static const int _normalDim         = 3;                    ///< Each normal is in 3D (x,y,z) (so w default to 1.0f)

/// Indices of vertex (from vertex buffer above)
static const GLshort _indexData[] = {
    // cube:
    0, 1, 2,    3, 2, 1,    // front
    4, 5, 6,    7, 6, 5,    // bottom
    8, 9, 10,   11, 10, 9,  // back
    12, 13, 14, 15, 14, 13, // top
    16, 17, 18, 19, 18, 17, // left
    20, 21, 22, 23, 22, 21, // right
    // plane
    24, 25, 26, 27,
};
static const int _sizeOfIndexData   = sizeof(_indexData);   ///< size of the _indexData;  ///< size array
/// Offset of the cube list :
static const int _offsetCube        = 0;
static const int _lenCubeList       = 36;                   ///< Number of indices for the cube list
/// Offset of the plane strip :
static const int _offsetPlane       = (_lenCubeList) * sizeof(_indexData[0]);
static const int _lenPlane          = 4;                    ///< Number of indices for the plane strip


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
    mNormalAttrib(-1),
    mModelToCameraMatrixUnif(-1),
    mCameraToClipMatrixUnif(-1),
    mVertexBufferObject(0),
    mIndexBufferObject(0),
    mVertexArrayObject(0),
    mCameraOrientation(),
    mCameraTranslation(0.0f, 2.0f, 6.0f),
    mDirToLight(0.866f, -0.5f, 0.0f, 0.0f), // Normalized vector !
    mLightIntensity(0.8f, 0.8f, 0.8f, 1.0f),
    mAmbientIntensity(0.2f, 0.2f, 0.2f, 1.0f) {
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

    // 3) Initialize the scene hierarchy
    initScene();

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
    // Enable blending transparency (and also the unused following OpenGL "SMOOTH" anti-aliasing)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*
    // Enable OpenGL "SMOOTH" polygon anti-aliasing
    // NO, does not work nicely; this would require to do depth sorted rendering
    // => prefer following modern multisampling MSAA or FSAA
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
*/
    // Query OpenGL for Multisampling support
    GLint multiSampling = 0;
    GLint numSamples  = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &multiSampling);
    if (0 != multiSampling) {
        glGetIntegerv(GL_SAMPLES, &numSamples);
        // Enable multisampling : MSAA/FSAA needs to be defined in NVIDIA/AMD/Intel driver panel
        glEnable(GL_MULTISAMPLE);
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST); // or GL_FASTEST
        mLog.notice() << "MultiSampling " << numSamples << "x";
    } else {
        mLog.warning() << "MultiSampling not working";
        glDisable(GL_MULTISAMPLE);
    }

    // Gamma correction to produce image in the sRGB colorspace
    glEnable(GL_FRAMEBUFFER_SRGB);
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
    mPositionAttrib = glGetAttribLocation(mProgram, "position");        // layout(location = 0) in vec4 position;
    mColorAttrib    = glGetAttribLocation(mProgram, "diffuseColor");    // layout(location = 1) in vec4 diffuseColor;
    mNormalAttrib   = glGetAttribLocation(mProgram, "normal");          // layout(location = 2) in vec4 normal;
    // Get location of uniforms - input variables of (vertex) shader
    // "Model to Camera" matrix, positioning the model into camera space
    // "Camera to Clip" matrix,  defining the perspective transformation
    mModelToCameraMatrixUnif    = glGetUniformLocation(mProgram, "modelToCameraMatrix");
    mCameraToClipMatrixUnif     = glGetUniformLocation(mProgram, "cameraToClipMatrix");
    mDirToLightUnif             = glGetUniformLocation(mProgram, "dirToLight");
    mLightIntensityUnif         = glGetUniformLocation(mProgram, "lightIntensity");
    mAmbientIntensityUnif       = glGetUniformLocation(mProgram, "ambientIntensity");

    // Set uniform values with our constants
    glUseProgram(mProgram);
    glUniform4fv(mLightIntensityUnif, 1, glm::value_ptr(mLightIntensity));
    glUniform4fv(mAmbientIntensityUnif, 1, glm::value_ptr(mAmbientIntensity));
    glUseProgram(0);
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
    glBufferData(GL_ARRAY_BUFFER, _sizeOfDataArray, _vertexData, GL_STATIC_DRAW);
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
    glEnableVertexAttribArray(mPositionAttrib); // layout(location = 0) in vec4 position;
    glEnableVertexAttribArray(mColorAttrib);    // layout(location = 1) in vec4 diffuseColor;
    glEnableVertexAttribArray(mNormalAttrib);   // layout(location = 2) in vec4 normal;

    // this tells the GPU witch part of the buffer to route to which attribute (shader input stream)
    glVertexAttribPointer(mPositionAttrib, _vertexDim, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(mColorAttrib, _colorDim, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(_offsetColors));
    glVertexAttribPointer(mNormalAttrib, _normalDim, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(_offsetNormals));
    // this tells OpenGL that vertex are pointed by index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

    glBindVertexArray(0);
}

/**
 * @brief  Initialize the scene hierarchy
 */
void Renderer::initScene() {
    Node::Ptr PlanePtr = Node::Ptr(new Node());
    PlanePtr->addDrawCall(Node::IndexedDrawCall(GL_TRIANGLE_STRIP, _lenPlane, GL_UNSIGNED_SHORT, _offsetPlane));
    mSceneHierarchy.addRootNode(PlanePtr);

    mModelPtr = Node::Ptr(new Node());
    mModelPtr->addDrawCall(Node::IndexedDrawCall(GL_TRIANGLES, _lenCubeList, GL_UNSIGNED_SHORT, _offsetCube));
    mModelPtr->move(glm::vec3(2.0f, 4.0f, -2.0f));
    mSceneHierarchy.addRootNode(mModelPtr);

    // TODO(SRombauts) assimp tests in progress

    // get a handle to the predefined STDOUT log stream and attach
    // it to the logging system. It remains active for all further
    // calls to aiImportFile(Ex) and aiApplyPostProcessing.
    struct aiLogStream stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
    aiAttachLogStream(&stream);

    // ... same procedure, but this stream now writes the
    // log messages to assimp_log.txt
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
    aiAttachLogStream(&stream);

    std::string         modelFile("data/cube.dae");
    Assimp::Importer    importer;
    // TODO(SRombauts) : revert CW to CCW then remove aiProcess_FlipWindingOrder
    const aiScene* pScene = importer.ReadFile(modelFile.c_str(), aiProcess_Triangulate | aiProcess_FlipWindingOrder);
    if (nullptr != pScene) {
        mLog.notice() << "importer.ReadFile(" << modelFile << ") sucessed";
        mLog.info() << "Meshes: " << pScene->mNumMeshes;
        // TODO(SRombauts)
        //for (unsigned int iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++)
        unsigned int iMesh = 0;
        if (iMesh < pScene->mNumMeshes) {
            aiMesh* pMesh = pScene->mMeshes[iMesh];
            if (nullptr != pMesh) {
                mLog.info() << " Vertices: " << pMesh->mNumVertices;
                mLog.info() << " Normals: " << (pMesh->HasNormals()?"true":"false");
                for (unsigned int iVertex = 0; iVertex < pMesh->mNumVertices; iVertex++) {
                    mLog.info() << "  Vertex: " << pMesh->mVertices[iVertex].x << ", " << pMesh->mVertices[iVertex].y << ", " << pMesh->mVertices[iVertex].z;
                    if (pMesh->HasNormals()) {
                        mLog.info() << "  Normal: " << pMesh->mNormals[iVertex].x << ", " << pMesh->mNormals[iVertex].y << ", " << pMesh->mNormals[iVertex].z;
                    }
                }
                mLog.info() << " Faces: " << pMesh->mNumFaces; // Indicies
                for (unsigned int iFace = 0; iFace < pMesh->mNumFaces; iFace++) {
                    aiFace& face = pMesh->mFaces[iFace];
                    mLog.info() << "  Indices: " << face.mNumIndices;
                    for (unsigned int iIndice = 0; iIndice < face.mNumIndices; iIndice++) {
                        mLog.info() << "   - " << face.mIndices[iIndice];
                    }
                }
            }
        }
    }  else {
        mLog.error() << "importer.ReadFile(" << modelFile << ") failed '" << importer.GetErrorString() << "'";
    }
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
 * @brief Move the camera from the given relative translation vector
 *
 * @param[in] aTranslation  3D Translation vector to add to the given camera position
 */
void Renderer::move(const glm::vec3& aTranslation) {
    // Get the rotation matrix from the orientation quaternion:
    glm::mat3 rotations = glm::mat3_cast(mCameraOrientation);
    // calculate relative translation into the current camera orientation
    const glm::vec3 relativeTranslation = (aTranslation * rotations);
    // and apply it to the current camera position
    mCameraTranslation += relativeTranslation;
    mLog.info() << "move: pos(" << mCameraTranslation.x << ","
                                << mCameraTranslation.y << ","
                                << mCameraTranslation.z << ")";
}

/**
 * @brief Pitch, rotate the camera vertically around its current relative horizontal X axis
 *
 * @param[in] aAngle    Rotation in radians
 */
void Renderer::pitch(float aAngle) {
    mLog.info() << "pitch(" << aAngle << ")";

    // Offset the given quaternion by the given angle (in radians) and normalized axis
    Node::rotateLeftMultiply(mCameraOrientation, aAngle, Node::UNIT_X_RIGHT); // left-multiply
}

/**
 * @brief Yaw, rotate the camera horizontally around its current relative vertical Y axis
 *
 * @param[in] aAngle    Rotation in radians
 */
void Renderer::yaw(float aAngle) {
    mLog.info() << "yaw(" << aAngle << ")";

    // Offset the given quaternion by the given angle (in radians) and normalized axis
    Node::rotateLeftMultiply(mCameraOrientation, aAngle, Node::UNIT_Y_UP); // left-multiply
}

/**
 * @brief Roll, rotate the camera around its current relative viewing Z axis
 *
 * @param[in] aAngle    Rotation in radians
 */
void Renderer::roll(float aAngle) {
    mLog.info() << "roll(" << aAngle << ")";

    // Offset the given quaternion by the given angle (in radians) and normalized axis
    Node::rotateLeftMultiply(mCameraOrientation, aAngle, Node::UNIT_Z_FRONT); // left-multiply
}

/**
 * @brief Calculate the new "worldToCameradMatrix" transformation matrix from Translations and Rotations
 *
 *  We want to apply translations first, then rotations, but matrix have to be multiplied in reverse order :
 * out = (rotations * translations) * in;
 *
 * @return "worldToCameradMatrix"
 */
glm::mat4 Renderer::transform() {
    // We begin to built the rotation matrix from the orientation quaternion:
    glm::mat4 rotations = glm::mat4_cast(mCameraOrientation);

    // Then we apply translations to the rotation matrix
    return glm::translate(rotations, -mCameraTranslation);
}

/**
 * @brief Move the move from the given relative translation vector
 *
 * @param[in] aTranslation  3D Translation vector to add to the given camera position
 */
void Renderer::modelMove(const glm::vec3& aTranslation) {
    mModelPtr->move(aTranslation);
}

/**
 * @brief Pitch, rotate the model vertically around its current relative horizontal X axis
 */
void Renderer::modelPitch(float aAngle) {
    mModelPtr->pitch(aAngle);
}

/**
 * @brief Yaw, rotate the model horizontally around its current relative vertical Y axis
 */
void Renderer::modelYaw(float aAngle) {
    mModelPtr->yaw(aAngle);
}

/**
 * @brief Roll, rotate the model around its current relative viewing Z axis
 */
void Renderer::modelRoll(float aAngle) {
    mModelPtr->roll(aAngle);
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
    glm::mat4 cameraToClipMatrix = glm::perspective<float>(45.0f, (aW / static_cast<float>(aH)), _zNear, _zFar);

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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(mProgram);

    // Bind the Vertex Array Object, bound to buffers with vertex position and colors
    glBindVertexArray(mVertexArrayObject);

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO(SRombauts) This camera related calculation need to go into a Camera class into the Scene
    // re-calculate the "World to Camera" matrix,
    glm::mat4 worldToCamerMatrix = transform();

    // mDirToLight have to be recalculated with each camera orientation change
    glm::vec4 lightDirCameraSpace = worldToCamerMatrix * mDirToLight;
    glUniform3fv(mDirToLightUnif, 1, glm::value_ptr(lightDirCameraSpace));

    // and initialize the "Model to Camera" matrix stack with it
    glutil::MatrixStack modelToCameraMatrixStack(worldToCamerMatrix);
    /////////////////////////////////////////////////////////////////////////////////////////////////

    // Use the matrix stack to manage the hierarchy of the scene
    mSceneHierarchy.draw(modelToCameraMatrixStack, mModelToCameraMatrixUnif);

    // Unbind the Vertex Array Object
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();    // Ask for refresh ; only needed if animation are present
}
