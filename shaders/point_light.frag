#version 330 core

/*******************
 * DATA STRUCTURES *
 *******************/
struct Light {
    vec3  colour;
    float intensity;
};

struct PointLight {
    Light base;
    vec3  position;
};

/*************
 * CONSTANTS *
 *************/
const float MIN_ATTENUATION_DIST = 10;
const float MAX_ATTENUATION_DIST = 100;

// Implicit output to OpenGL.
out vec4 outColour;

// Geometry buffer textures uniforms.
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D gDepth;

// Camera uniforms.
uniform mat4 matView;
uniform mat4 matProjection;

// Light input uniform.
uniform PointLight pointLight;

// Input from vertex shader.
in vec2 fTexCoord;

float interpolateAttenuation(float distance) {
    float result = 0.0f;

    if (distance <= MIN_ATTENUATION_DIST) {
        result = 1.0f;
    } else if (distance < MAX_ATTENUATION_DIST) {
        result = (MAX_ATTENUATION_DIST - distance) / (MAX_ATTENUATION_DIST - MIN_ATTENUATION_DIST);
    }

    return result;
}

// Returns the reflected RGB colour of a point light applied to a single fragment.
vec3 renderPointLight(PointLight p, vec3 fragDiffuse, vec3 fragSpecular, vec4 fragNormal, vec4 fragPos) {
    // Fragment-to-light vector.
    vec4 ftl = vec4(p.position, 1.0f) - fragPos;

    vec4 ftlDir = normalize(ftl);

    float lambertian = max(dot(ftlDir, fragNormal), 0.0f);

    float specular = 0.0f;

    if (lambertian > 0.0f) {
       vec3 viewDir = normalize((inverse(matView) * vec4(0.0, 0.0, 1.0, 0.0))).xyz;

       vec3 halfDir = normalize(ftlDir.xyz + viewDir);
       float specFactor = max(dot(halfDir, fragNormal.xyz), 0.0f);
       specular = pow(specFactor, 16.0f);
    }

    float attenuation = interpolateAttenuation(length(ftl));

    return p.base.intensity * (specular * fragSpecular + lambertian * fragDiffuse) * attenuation * p.base.colour;
}

// Converts a 2D screen-space coordinate to a 3D world-space coordinate.
vec4 fragPositionFromDepthBuffer(vec2 screenPosition) {
    // Get the depth value from the depth buffer.
    float depth = texture(gDepth, screenPosition).r;

    vec4 ndc;
    ndc.xy = screenPosition * 2.0f - 1.0f;
    ndc.z = depth * 2.0f - 1.0f;
    ndc.w = 1.0f;

    vec4 homog = inverse(matProjection * matView) * ndc;
    homog /= homog.w;

    return homog;
}

void main() {
    // Get the data from the geometry buffer.
    vec4 fragNormal   = texture(gNormal, fTexCoord);
    vec3 fragDiffuse  = texture(gDiffuse, fTexCoord).rgb;
    vec3 fragSpecular = texture(gSpecular, fTexCoord).rgb;

    // Obtain the world-space fragment position.
    vec4 fragPosition = fragPositionFromDepthBuffer(fTexCoord);

    vec3 finalFragColour = renderPointLight(pointLight, fragDiffuse, fragSpecular, fragNormal, fragPosition);

    // Clamp to a sensible range.
    finalFragColour = clamp(finalFragColour, vec3(0.0f), vec3(1.0f));

    outColour = vec4(finalFragColour, 1.0f);
}
