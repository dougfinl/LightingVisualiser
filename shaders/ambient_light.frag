#version 330 core

// Implicit output to OpenGL.
out vec4 outColour;

// Geometry buffer textures uniform.
uniform sampler2D gDiffuse;

// Ambient light intensity.
uniform float ambientIntensity;

// Input from vertex shader.
in vec2 fTexCoord;


void main() {
	gl_FragColor = texture(gDiffuse, fTexCoord) * ambientIntensity;
}
