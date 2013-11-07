#version 330

// 1 entrée (smoothColor)
smooth in vec4 smoothColor;

// 1 sortie (outputColor)
out vec4 outputColor;

void main()
{
    outputColor = smoothColor;
}
