#version 330 core

layout (points) in;
layout (line_strip, max_vertices=2) out;

// Output to fragment shader.
out vec4 fLightColour;

uniform mat4 camera;

const float LINE_LENGTH = 5.0f;

struct Light {
    vec3  colour;
    float intensity;
};

struct PointLight {
    Light base;
    vec3  position;
};

struct SpotLight {
    PointLight base;
    vec3       direction;
};

uniform SpotLight spotLight;


void main() {
    // Create a line for each spot light.
    vec4 lightPosition  = camera * vec4(spotLight.base.position, 1.0f);
    vec4 lightDirection = camera * vec4(spotLight.direction, 0.0f);

    fLightColour = vec4(spotLight.base.base.colour, 1.0f) * spotLight.base.base.intensity;

    // Line start
    gl_Position = lightPosition;
    EmitVertex();

    // Line end
    gl_Position = lightPosition + (lightDirection * LINE_LENGTH);
    EmitVertex();

    EndPrimitive();
}
