#version 330

// 3 input streams "attributes" (model vertex position, color and normals)
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 diffuseColor;
layout(location = 2) in vec3 normal;

// 2 output streams (default gl_Position, and smoothColor)
smooth out vec4 smoothColor;

// 5 input uniform (matrix of transformation, and light parameters)
uniform mat4 modelToCameraMatrix;   // "Model to Camera" matrix, positioning the model into camera space (the "view" matrix)
uniform mat4 cameraToClipMatrix;    // "Camera to Clip" matrix,  defining the perspective projection
uniform vec3 dirToLight;            // Vector of directional light orientation (oriented toward the light)
uniform vec4 lightIntensity;        // Directional light intensity and color
uniform vec4 ambientIntensity;      // Ambiant light intensity and color

void main()
{
    // Vertex positions
    vec4 cameraPos   = modelToCameraMatrix * position;   // Convert model position into camera space coordinates
         gl_Position = cameraToClipMatrix  * cameraPos;  // Convert camera position into clip space coordinates

    // Vertex normals
    vec3 normCamSpace = normalize(mat3(modelToCameraMatrix) * normal);

    // Light incidence
    float cosAngIncidence = dot(normCamSpace, dirToLight);
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
   
    // Resulting color
    // TODO HDR (divide by a max value)
    smoothColor = (diffuseColor * lightIntensity * cosAngIncidence)
                + (diffuseColor * ambientIntensity);
}
