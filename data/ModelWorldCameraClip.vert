#version 330

// 2 entrées (position & color)
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

// 2 sorties (gl_Position & smoothColor)
smooth out vec4 smoothColor;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 cameraToClipMatrix;

void main()
{
    vec4 temp        = modelToWorldMatrix  * position;
         temp        = worldToCameraMatrix * temp;
         gl_Position = cameraToClipMatrix  * temp;
         
    smoothColor = color;
}
