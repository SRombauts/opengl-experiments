#version 330

// 2 input streams "attributes" (model vertex position & color)
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

// 2 output streams (default gl_Position & smoothColor)
smooth out vec4 smoothColor;

// 2 input uniform (matrix of transformation)
uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;

void main()
{
    vec4 cameraPos   = modelToCameraMatrix * position;   // Convert model position into camera space coordinates
         gl_Position = cameraToClipMatrix  * cameraPos;  // Convert camera position into clip space coordinates
         
    smoothColor = color;
}
