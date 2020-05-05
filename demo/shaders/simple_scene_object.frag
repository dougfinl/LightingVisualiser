#version 330 core

out vec4 outColour;

// Inputs from vertex shader.
in vec2 fTexCoord;

// Mesh material.
uniform struct Material {
    vec3 colourDiffuse;
    bool hasTexDiffuse;
} mat;

// Mesh texture samplers.
uniform sampler2D texDiffuse;


void main() {
    if (mat.hasTexDiffuse) {
        outColour = texture(texDiffuse, fTexCoord);
    } else {
        outColour = vec4(mat.colourDiffuse, 1.0f);
    }
}
