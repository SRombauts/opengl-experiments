#version 330

// 1 input (smoothColor)
smooth in vec4 smoothColor;

// 1 output (outputColor)
out vec4 outputColor;

void main()
{
    outputColor = smoothColor;
}
