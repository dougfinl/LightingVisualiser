#version 330 core

// Inputs from mesh (full-screen quad).
layout (location=0) in vec3 position;
layout (location=2) in vec2 texCoord;

// Outputs to fragment shader.
out vec2 fTexCoord;


void main() {
    gl_Position = vec4(position, 1.0f);

    fTexCoord = texCoord;
}

