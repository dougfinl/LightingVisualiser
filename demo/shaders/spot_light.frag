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

struct SpotLight {
    PointLight      base;
    vec3            direction;
    float           beamAngle;
    float           fieldAngle;
    bool            hasGobo;
    mat4            goboMatrix;
    mat4            matrix;
    sampler2D       texGobo;
    sampler2DShadow texShadowMap;
};

/*************
 * CONSTANTS *
 *************/
// Matrix for converting from NDC to texture coordinate.
const mat4 lightBiasMatrix = mat4(
    vec4(0.5, 0.0, 0.0, 0.0),
    vec4(0.0, 0.5, 0.0, 0.0),
    vec4(0.0, 0.0, 0.5, 0.0),
    vec4(0.5, 0.5, 0.5, 1.0)
);

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
uniform SpotLight spotLight;

// Input from vertex shader.
in vec2 fTexCoord;


float interpolateSpotEdge(float beamAngle, float fieldAngle, float angle) {
    float result = 0.0f;

    if (angle < beamAngle) {
        // Calculate the edge value, in the range 0.5-1.0.
        float epsilon = 0 - beamAngle;
        result = clamp((angle - beamAngle) / epsilon, 0.0, 1.0);
        result = result * 0.5 + 0.5;
    } else if (angle < fieldAngle) {
        // Calculate the edge value, in the range 0.1-0.5.
        float epsilon = beamAngle - fieldAngle;
        result = clamp((angle - fieldAngle) / epsilon, 0.0, 1.0);
        result = result * (0.5 - 0.1) + 0.1;
    }

    return result;
}

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

// Returns the reflected RGB colour of a spot light applied to a single fragment.
vec3 renderSpotLight(SpotLight s, vec3 fragDiffuse, vec3 fragSpecular, vec4 fragNormal, vec4 fragPos) {
    // Normalised fragment-to-light vector.
    vec4 ftl = normalize(vec4(s.base.position, 1.0f) - fragPos);

    // Angle between light and fragment (radians).
    float lfAngle = acos(dot(-ftl, vec4(s.direction, 1.0f)));

    // Check if the current fragment lies inside the light.
    if (2*lfAngle < s.fieldAngle) {
        // Rneder the fragment as per a point light.
        vec3 light = renderPointLight(s.base, fragDiffuse, fragSpecular, fragNormal, fragPos);

        // Apply the spot's diffused edge.
        light *= interpolateSpotEdge(s.beamAngle, s.fieldAngle, 2*lfAngle);

        if (s.hasGobo) {
            vec4 fragPosGoboSpace = s.goboMatrix * fragPos;
            vec4 goboTexCoord = lightBiasMatrix * fragPosGoboSpace;

            // Darken fragments masked by the gobo.
            vec3 c = textureProj(s.texGobo, goboTexCoord).xyz;
            if (length(c) < 0.5) {
                light *= 0;
            }
        }

        // Apply the perspective shadow.
        vec4 fragPosLightSpace = s.matrix * fragPos;
        vec4 shadowTexCoord = lightBiasMatrix * fragPosLightSpace;

        vec4 bias = vec4(0, 0, 0.05, 0);
        float shadowValue = textureProj(s.texShadowMap, shadowTexCoord - bias);
        light *= shadowValue;

        return light;
    } else {
        return vec3(0.0f);
    }
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

    vec3 finalFragColour = renderSpotLight(spotLight, fragDiffuse, fragSpecular, fragNormal, fragPosition);

    // Clamp to a sensible range.
    finalFragColour = clamp(finalFragColour, vec3(0.0f), vec3(1.0f));

    outColour = vec4(finalFragColour, 1.0f);
}

