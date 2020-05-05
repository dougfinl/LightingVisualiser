#version 330 core

// Model inputs.
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoord;
layout (location=3) in mat4 modelTransformation; // Note: mat4 takes up 4 locations (4x vec4)

// Outputs to fragment shader.
out vec2 fTexCoord;
out vec3 fNormal;

uniform mat4 camera;

void main() {
    gl_Position = camera * modelTransformation * vec4(position, 1.0f);

    fNormal   = (modelTransformation * vec4(normal, 0.0f)).xyz;
    fTexCoord = texCoord;
}
