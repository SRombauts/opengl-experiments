/**
 * @file    Renderer.h
 * @ingroup opengl-experiments
 * @brief   Management of OpenGL drawing/rendering
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "LoggerCpp/LoggerCpp.h"

#include "Utils/Utils.h"

#include <glload/gl_3_3_comp.h> // GLuint, GLenum
#include <glm/glm.hpp>          // glm::mat4, glm::vec3...
#include <glutil/MatrixStack.h>

#include <vector>


/**
 * @brief Management of OpenGL drawing/rendering
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    // called by Input freeglut callbacks
    void reshape(int aW, int aH);
    void display();

    // called by Input::checkKeys()
    // camera:
    void up();
    void down();
    void left();
    void right();
    void front();
    void back();
    void rotate(int aDeltaX, int aDeltaY);
    // model:
    void modelUp();
    void modelDown();
    void modelLeft();
    void modelRight();
    void modelFront();
    void modelBack();
    void modelRotate(int aDeltaX, int aDeltaY);

private:
    // Initialization
    void init();
    void compileShader(std::vector<GLuint>& aShaderList, const GLenum aShaderType, const char* apShaderFilename) const;
    void initProgram();
    void initVertexArrayObject();
    void uninitVertexArrayObject();

    glm::mat4 transform();
    glm::mat4 modelTransform();

    void drawPlane(glutil::MatrixStack& aModelToWorldMatrixStack);
    void drawCube(glutil::MatrixStack& aModelToWorldMatrixStack);

    void calculateFPS();

private:
    Log::Logger mLog;                   ///< Logger object to output runtime information

    GLuint mProgram;                    ///< OpenGL program containing compiled and linked shaders
    GLuint mPositionAttrib;             ///< Location of the "position" vertex shader attribute (input stream)
    GLuint mColorAttrib;                ///< Location of the "color" vertex shader attribute (input stream)
    GLuint mModelToWorldMatrixUnif;     ///< Location of the "modelToWorldMatrix"  vertex shader uniform input variable
    GLuint mWorldToCameraMatrixUnif;    ///< Location of the "worldToCameraMatrix" vertex shader uniform input variable
    GLuint mCameraToClipMatrixUnif;     ///< Location of the "cameraToClipMatrix"  vertex shader uniform input variable

    GLuint mVertexBufferObject;         ///< Vertex buffer object containing the data of our mesh
    GLuint mIndexBufferObject;          ///< Index buffer object containing the indices of vertices of our mesh
    GLuint mVertexArrayObject;          ///< Vertex array object

    // TODO(SRombauts) use a quaternion
    glm::vec3   mCameraRotation;        ///< Angles of rotation of the camera
    glm::vec3   mCameraTranslation;     ///< Vector of translation of the camera

    glm::vec3   mModelRotation;         ///< Angles of rotation of the model
    glm::vec3   mModelTranslation;      ///< Vector of translation of the model

private:
    /// disallow copy constructor and assignment operator
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};

