/**
 * @file    Renderer.cpp
 * @ingroup Main
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Main/Renderer.h"
#include "Main/MatrixStack.h"
#include "Main/ShaderProgram.h"
#include "Utils/Exception.h"
#include "Utils/Measure.h"
#include "Utils/String.h"

#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::perspective, glm::rotate, glm::translate

#include <assimp/cimport.h>     // Log Stream
#include <assimp/Importer.hpp>  // Open Asset Importer
#include <assimp/postprocess.h> // Post processing flags

#include <fstream>      // NOLINT(readability/streams) for shader files
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>

#include <cmath>    // cos, sin, tan

/* We use a standard "Right Hand Coordinate System", and the unit is the meter:
static const float X_RIGHT  = 1.0f;     ///< Right coordinate
static const float X_LEFT   = -1.0f;    ///< Left coordinate
static const float Y_TOP    = 1.0f;     ///< Top coordinate
static const float Y_BOTTOM = -1.0f;    ///< Bottom coordinate
static const float Z_FRONT  = 1.0f;     ///< Front coordinate
static const float Z_BACK   = -1.0f;    ///< Back coordinate
*/

static const float _zNear           = 0.1f;     ///< Z coordinate or the near/front frustum plane from which to render
static const float _zFar            = 10000.0f; ///< Z coordinate or the far/back frustum plane to which to render


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
    mCameraOrientation(),
    mCameraTranslation(0.0f, 0.0f, 30.0f),
    mDirToLight(0.866f, -0.5f, 0.0f, 0.0f), // Normalized vector!
    mLightIntensity(0.8f, 0.8f, 0.8f, 1.0f),
    mAmbientIntensity(0.2f, 0.2f, 0.2f, 1.0f),
    mScreenWidth(0),
    mScreenHeight(0),
    mScreenCenterOffset(2.0f) {
    init();
}

/**
 * @brief Destructor
 */
Renderer::~Renderer() {
    glDeleteProgram(mProgram);
}

/**
 * @brief Initialization
 */
void Renderer::init() {
    // 1) compile shaders and link them in a program
    initProgram();

    // 2) Initialize the scene hierarchy
    initScene();

    // 2) Initialize more OpenGL option
    // Face Culling : We use the OpenGL default Counter Clockwise Winding order (GL_CCW)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    // Enable blending transparency (and also the unused following OpenGL "SMOOTH" anti-aliasing)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // NOTE OpenGL "SMOOTH" polygon anti-aliasing, does NOT work nicely; it requires to do depth sorted rendering
    //   => prefer following modern multisampling MSAA or FSAA
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
    ShaderProgram   shaderProgram;

    // Compile the shader files (into intermediate compiled object)
    // and Link them in a program (into the final executable to send to the GPU)
    mLog.debug() << "initProgram: compiling shaders and linking program...";
    mProgram = shaderProgram.makeProgram("data/ModelWorldCameraClip.vert", "data/PassthroughColor.frag");

    // Get location of (vertex) attributes (input streams of (vertex) shader
    /// @todo test (THROW) attribute and uniform != -1
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
 * @brief  Initialize the scene hierarchy
 */
void Renderer::initScene() {
    // get a handle to the predefined STDOUT log stream and attach
    // it to the logging system. It remains active for all further
    // calls to aiImportFile(Ex) and aiApplyPostProcessing.
    /// @todo redirect to a LoggerCpp stream
    struct aiLogStream stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
    aiAttachLogStream(&stream);

    // ... same procedure, but this stream now writes the log messages to assimp_log.txt
    // stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
    // aiAttachLogStream(&stream);

    // We use a text file to tell which test mesh to load
    std::string importFilename = "data/import.txt";
    mLog.debug() << "initScene(" << importFilename << ")...";
    std::ifstream importFile(importFilename.c_str());
    if (false == importFile.is_open()) {
        mLog.critic() << "initScene: unavailable file \"" << importFilename << "\"";
        UTILS_THROW("compileShader: unavailable file \"" << importFilename << "\"");
    }

    /// @todo multi-line multi-mesh loading
    std::string modelFile;
    if (std::getline(importFile, modelFile)) {
        Utils::trim(modelFile);

        mLog.notice() << "initScene(\"" << importFilename << "\") modelFile=\"" << modelFile << "\"";

        // Load the mesh of our main movable model (a colored cube by default), and add it to the Scene hierarchy
        Node::Ptr HierarchyPtr = loadFile(modelFile.c_str());
        mSceneHierarchy.addRootNode(HierarchyPtr);
        /// @todo here we get to the Cuboid & Cube models => use a dictionary (map) to get models by names
        mModelPtr  = HierarchyPtr;
        mModelPtr->move(glm::vec3(-3.0f, -1.0f, -4.0f));
        mModelPtr->yaw(1.57f); // 90° horizontally arround y (to face right)
        mModelPtr->roll(0.2f); // arround z
        mModelPtr->setRotationalSpeed(glm::vec3(-0.05f, -0.3f, 0.0f)); // pitch, yaw, roll
        mModelPtr->setLinearSpeed(glm::vec3(0.0f, 0.0f, 3.0f));
        mTurretPtr = HierarchyPtr->getChildren().front();
        mTurretPtr->setRotationalSpeed(glm::vec3(0.0f, 0.8f, 0.0f));
    } else  {
        mLog.critic() << "initScene: no model file in \"" << importFilename << "\"";
        UTILS_THROW("compileShader: no model file in \"" << importFilename << "\"");
    }

    // Load an experimental cockpit => toward a camera view in world
    Node::Ptr CockpitPtr = loadFile("data/cockpit.dae");
    CockpitPtr->move(glm::vec3(0.0f, 0.0f, 0.0f));
//  CockpitPtr->yaw(3.14f); // 180° horizontally arround y (to face back)
    mSceneHierarchy.addRootNode(CockpitPtr);

    // Load a ground/plane for some kind of fixe reference
    Node::Ptr PlanePtr = loadFile("data/plane.dae");
    mSceneHierarchy.addRootNode(PlanePtr);
}

/**
 * @brief Load of Mesh file and put it on a new Node
 *
 * @param[in] apFilename    Name of the mesh file to load (must be supported by assimp)
 *
 * @return A pointer to the new Node, or throw a std::exception if none loaded
 */
Node::Ptr Renderer::loadFile(const char* apFilename) {
    Node::Ptr           NodePtr;
    Utils::Measure      measure;
    Assimp::Importer    importer;
    mLog.notice() << "loadFile(" << apFilename << ")...";

    // Read the mesh file
    const aiScene* pScene = importer.ReadFile(apFilename, aiProcessPreset_TargetRealtime_Fast);
    if ( (nullptr != pScene) && (0 == (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) ) {
        mLog.info() << "Meshes: " << pScene->mNumMeshes;

        aiNode* pNode = pScene->mRootNode;
        // Load recursively all the nodes
        NodePtr = loadNode(pScene, pNode);
    }  else {
        mLog.critic() << "loadFile(" << apFilename << ") failed '" << importer.GetErrorString() << "'";
        UTILS_THROW("loadFile(" << apFilename << ") failed '" << importer.GetErrorString() << "'");
    }
    time_t diffUs = measure.diff();
    mLog.notice() << "loadFile(" << apFilename << ") done in " << diffUs/1000000 << "." << diffUs/1000 << "s";

    return NodePtr;
}

/**
 * @brief Load recursively a node and its Meshes and put it on a new Node
 *
 * @param[in] apScene   Pointer to the Assimp Scene
 * @param[in] apNode    Pointer to the Assimp Node
 *
 * @return A pointer to the new Node, or throw a std::exception if none loaded
 */
Node::Ptr Renderer::loadNode(const aiScene* apScene, const aiNode* apNode) {
    Node::Ptr NodePtr;
    assert(nullptr != apNode);

    // If the Node has at least one Mesh or more than one Child
    /// @todo Loading Cameras and Lights
    if ( (1 <= apNode->mNumMeshes) || (2 < apNode->mNumChildren) ) {
        NodePtr.reset(new Node(apNode->mName.C_Str()));
        mLog.info() << "Node '" << apNode->mName.C_Str() << "'";

        // Decompose the Node traformation matrix with no scaling into its original components
        aiQuaternion rotation;
        aiVector3D position;
        apNode->mTransformation.DecomposeNoScaling(rotation, position);
        NodePtr->setOrientationQuaternion(rotation.w, rotation.x, rotation.y, rotation.z);
        NodePtr->setTranslationVector(position.x, position.y, position.z);

        // Load all meshes of the current Node
        for (unsigned int iMesh = 0; iMesh < apNode->mNumMeshes; ++iMesh) {
            unsigned int idxMesh = apNode->mMeshes[iMesh];
            aiMesh* pMesh = apScene->mMeshes[idxMesh];
            assert(nullptr != pMesh);
            const size_t nbSet = 1 + (pMesh->HasNormals()?1:0) + (pMesh->HasVertexColors(0)?1:0);
            const size_t nbOfData = pMesh->mNumVertices * nbSet;
            std::vector<glm::vec3> vertexData;
            vertexData.reserve(nbOfData);

            mLog.info() << " Mesh '" << pMesh->mName.C_Str() << "'";
            mLog.info() << "  Vertices: " << pMesh->mNumVertices;
            mLog.info() << "  Colors: "   << (pMesh->HasVertexColors(0)?"true":"false");
            mLog.info() << "  Normals: "  << (pMesh->HasNormals()?"true":"false");
            for (unsigned int iVertex = 0; iVertex < pMesh->mNumVertices; ++iVertex) {
                // mLog.info() << "   Vertex: " << pMesh->mVertices[iVertex].x
                //             << ", " << pMesh->mVertices[iVertex].y << ", " << pMesh->mVertices[iVertex].z;
                vertexData.push_back(glm::vec3(pMesh->mVertices[iVertex].x,
                                                pMesh->mVertices[iVertex].y,
                                                pMesh->mVertices[iVertex].z));
                if (pMesh->HasVertexColors(0)) {
                    // mLog.info() << "   Colors: " << pMesh->mColors[0][iVertex].r
                    //             << ", " << pMesh->mColors[0][iVertex].g << ", " << pMesh->mColors[0][iVertex].b;
                    vertexData.push_back(glm::vec3(pMesh->mColors[0][iVertex].r,
                                                    pMesh->mColors[0][iVertex].g,
                                                    pMesh->mColors[0][iVertex].b));
                } else {
                    // NOTE if no colors, use pure white
                    vertexData.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
                }
                if (pMesh->HasNormals()) {
                    // mLog.info() << "   Normal: " << pMesh->mNormals[iVertex].x
                    //             << ", " << pMesh->mNormals[iVertex].y << ", " << pMesh->mNormals[iVertex].z;
                    vertexData.push_back(glm::vec3(pMesh->mNormals[iVertex].x,
                                                    pMesh->mNormals[iVertex].y,
                                                    pMesh->mNormals[iVertex].z));
                } else {
                    // NOTE if no normals, we cannot render properly, but we can still see something
                    vertexData.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }

            // If only triangles :
            const size_t nbOfIndex = pMesh->mNumFaces * 3;
            if (65536 < nbOfIndex) {
                /// @todo if there is more than 64k indices, switch to GL_UNSIGNED_INT !
                mLog.critic() << "loadNode: too many indices for SHORT (" << nbOfIndex << " > " << 65536 << ")";
                UTILS_THROW("loadNode: too many indices for SHORT (" << nbOfIndex << " > " << 65536 << ")");
            }
            std::vector<GLshort> vertexIndex;
            vertexIndex.reserve(nbOfIndex);

            mLog.info() << " Faces: " << pMesh->mNumFaces;
            for (unsigned int iFace = 0; iFace < pMesh->mNumFaces; ++iFace) {
                aiFace& face = pMesh->mFaces[iFace];
                // mLog.info() << "  Indices: " << face.mNumIndices;
                assert(3 == face.mNumIndices);
                for (unsigned int iIndice = 0; iIndice < face.mNumIndices; ++iIndice) {
                    // mLog.info() << "   - " << face.mIndices[iIndice];
                    vertexIndex.push_back(static_cast<GLshort>(face.mIndices[iIndice]));
                }
            }

            /// @todo The following API is not good => short<->int
            // Generate a Mesh objet to draw the imported model
            Mesh::Ptr MeshPtr(new Mesh(pMesh->mName.C_Str(), GL_TRIANGLES, vertexIndex.size(), GL_UNSIGNED_SHORT, 0));
            // Generate a VBO/VBI & VAO in GPU memory with those data
            MeshPtr->genOpenGlObjects(vertexData, vertexIndex, mPositionAttrib, mColorAttrib, mNormalAttrib);
            // here vertexData and vertexIndex are of no more use, std::vector memory will be deallocated
            // here pScene is of no more use, Assimp::Importer will release it

            NodePtr->addMesh(std::move(MeshPtr));
        }

        // Load all children of the current Node recursively
        mLog.info() << " Children: " << apNode->mNumChildren;
        for (unsigned int iChild = 0; iChild < apNode->mNumChildren; ++iChild) {
            const aiNode* pChildNode = apNode->mChildren[iChild];
            // Load a child Node...
            Node::Ptr ChildNodePtr = loadNode(apScene, pChildNode);
            if (ChildNodePtr) {
                // and add it to the current Node (if not empty)
                NodePtr->addChildNode(ChildNodePtr);
            }
        }
    } else if (1 == apNode->mNumChildren) {
        // No Mesh and only one child: skip this Node of the hierarchy! (ex. Root Scene Node)
        /// @todo Accumulate matrix transformation not to loose relative positionning if any
        mLog.debug() << "Skipped Node '" << apNode->mName.C_Str() << "'";
        const aiNode* pChildNode = apNode->mChildren[0];
        NodePtr = loadNode(apScene, pChildNode);
    }
    return NodePtr;
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
 * @brief Set directly the quaternion of camera orientation (from Oculus HMD)
 *
 * @param[in] aCameraOrientation    Quaternion of camera orientation
 */
void Renderer::setCameraOrientation(const glm::fquat& aCameraOrientation) {
    mCameraOrientation = aCameraOrientation;
}

/**
 * @brief Calculate the new "worldToCameradMatrix" transformation matrix from Translations and Rotations
 *
 *  We want to apply translations first, then rotations, but matrix have to be multiplied in reverse order :
 * out = (rotations * translations) * in;
 *
 * @param[in] aIdxEye   Index of the eye (0: left, 1: right)
 *
 * @return "worldToCameradMatrix"
 */
glm::mat4 Renderer::getWorldToCameraMatrix(int aIdxEye) {
    // We begin to built the rotation matrix from the conjugate of the orientation quaternion:
    glm::mat4 rotations = glm::mat4_cast(glm::conjugate(mCameraOrientation));

    // Then we apply head/global translations to the rotation matrix
    glm::mat4 worldToHeadMatrix = glm::translate(rotations, -mCameraTranslation);

    /// @todo use a neck-head-eye model to get eye translation vector
//  float eye = (0 == aIdxEye)?(-0.0315f):(0.0315f);        // Default Lens separation & IPD of 63.5mm
    float eye = (0 == aIdxEye)?(mScreenCenterOffset):(-mScreenCenterOffset);    // Calculated screen center offset

    glm::vec3 eyeTranslation(eye, 0.0f, 0.0f);

    // And the, we apply the eye translation and return the resulting matrix
    return glm::translate(worldToHeadMatrix, eyeTranslation);
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
    mTurretPtr->pitch(aAngle);
}

/**
 * @brief Yaw, rotate the model horizontally around its current relative vertical Y axis
 */
void Renderer::modelYaw(float aAngle) {
    mTurretPtr->yaw(aAngle);
}

/**
 * @brief Roll, rotate the model around its current relative viewing Z axis
 */
void Renderer::modelRoll(float aAngle) {
    mTurretPtr->roll(aAngle);
}

/**
 * @brief Reshape method
 *
 *  Called once at the start of the rendering, and then for each window resizing.
 *
 * @param[in] aW    Useful window width
 * @param[in] aH    Useful window height
 */
void Renderer::reshape(int aW, int aH) {
    mLog.info() << "reshape(" << aW << "," << aH << ")";
    mScreenWidth = aW;
    mScreenHeight = aH;

    // Define the "Camera to Clip" matrix for the perspective transformation
    glm::mat4 cameraToClipMatrix = glm::perspective<float>(45.0f, ((aW/2) / static_cast<float>(aH)), _zNear, _zFar);

    // Set uniform values with the new "Camera to Clip" matrix
    glUseProgram(mProgram);
    glUniformMatrix4fv(mCameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
    glUseProgram(0);
}

/**
 * @brief Display method
 */
void Renderer::display() {
    // mLog.debug() << "displayCallback()";

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the linked program of compiled shaders
    glUseProgram(mProgram);

    // Stereo rendering
    for (int idxEye = 0; idxEye <= 1; ++idxEye) {
        /// @todo Use a config class for each eye
        if (0 == idxEye) {
            // Left eye rendering :
            glViewport(0, 0, (GLsizei)(mScreenWidth/2), (GLsizei)mScreenHeight);
        } else {
            // Right eye rendering :
            glViewport((GLsizei)(mScreenWidth/2), 0, (GLsizei)(mScreenWidth/2), (GLsizei)mScreenHeight);
        }

        ////////////////////////////////////////////////////////////////////////////////////////
        /// @todo This camera related calculation need to go into a Camera class into the Scene
        // re-calculate the "World to Camera" matrix,
        glm::mat4 worldToCamerMatrix = getWorldToCameraMatrix(idxEye);

        // mDirToLight have to be recalculated with each camera orientation change
        glm::vec4 lightDirCameraSpace = worldToCamerMatrix * mDirToLight;
        glUniform3fv(mDirToLightUnif, 1, glm::value_ptr(lightDirCameraSpace));

        // and initialize the "Model to Camera" matrix stack with it
        MatrixStack modelToCameraMatrixStack(worldToCamerMatrix);
        ////////////////////////////////////////////////////////////////////////////////////////

        // Use the matrix stack to manage the hierarchy of the scene
        mSceneHierarchy.draw(modelToCameraMatrixStack, mModelToCameraMatrixUnif);
    }

    // Unbind the Vertex Program
    glUseProgram(0);

    glFlush();
}
