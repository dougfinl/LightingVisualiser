#version 330 core

layout (location=0) in vec3 position;
layout (location=2) in vec2 texCoord;
layout (location=3) in mat4 modelTransformation;

uniform mat4 camera;

// Outputs to fragment shader.
out vec2 fTexCoord;


void main() {
    gl_Position = camera * modelTransformation * vec4(position, 1.0f);

    fTexCoord = texCoord;
}
