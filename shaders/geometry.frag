#version 330 core

// Outputs to geometry buffer.
layout (location=0) out vec3 outNormal;
layout (location=1) out vec4 outDiffuse;
layout (location=2) out vec4 outSpecular;

// Inputs from vertex shader.
in vec2 fTexCoord;
in vec3 fNormal;

// Mesh material.
uniform struct Material {
    vec3 colourDiffuse;
    bool hasTexDiffuse;
    vec3 colourSpecular;
    bool hasTexSpecular;
} mat;

// Mesh texture samplers.
uniform sampler2D texDiffuse;
uniform sampler2D texSpecular;

void main() {
    outNormal = normalize(fNormal);

    if (mat.hasTexDiffuse)
        outDiffuse = texture(texDiffuse, fTexCoord);
    else
        outDiffuse = vec4(mat.colourDiffuse, 1.0f);

    if (mat.hasTexSpecular)
        outSpecular = texture(texSpecular, fTexCoord);
    else
        outSpecular = vec4(mat.colourSpecular, 1.0f);
}
