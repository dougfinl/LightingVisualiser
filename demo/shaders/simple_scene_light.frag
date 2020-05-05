#version 330 core

// Implicit output to OpenGL.
out vec4 outColour;

// Input from geometry shader.
in vec4 fLightColour;

void main() {
    outColour = fLightColour;
}
