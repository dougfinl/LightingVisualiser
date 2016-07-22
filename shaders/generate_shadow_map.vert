#version 330 core

// Model inputs.
layout (location=0) in vec3 position;
layout (location=3) in mat4 modelTransformation; // Note: mat4 takes up 4 locations (4x vec4)

uniform mat4 matSpotLight;

void main() {
    gl_Position = matSpotLight * modelTransformation * vec4(position, 1.0f);
}
