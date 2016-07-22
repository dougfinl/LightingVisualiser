//
// Created by dougfinl on 21/11/15.
//

#ifndef VISUALISER_LIGHTINGRIG_H
#define VISUALISER_LIGHTINGRIG_H

#include <vector>
#include <glm/glm.hpp>
#include "../stage/Texture.h"
#include "../rendering/ShadowBuffer.h"


namespace visualiser {
namespace lighting {

//
// Represents a single, abstract light source.
//
struct Light {
    uint32_t address;
    glm::vec3 colour;
    float intensity;
};

//
// Represents a single point light source, in which light is emitted in all directions.
//
struct PointLight {
    Light base;
    glm::vec3 position;
};

//
// Represents a single spot light source, in which light is emitted in a single direction with a particular angle of
// spread.
//
struct SpotLight {
    PointLight base;
    glm::vec3 direction;
    float beamAngle;
    float fieldAngle;
    bool hasGobo;
    float goboRotation;
    stage::Texture *gobo;
    glm::mat4 goboMatrix;
    glm::mat4 matrix;
    rendering::ShadowBuffer *shadowBuffer;
};

//
// Stores a collection of point lights and spot lights.
//
class LightingRig {
    public:
        // Creates an empty lighting rig.
        LightingRig();
        // Copy constructor
        explicit LightingRig(const LightingRig &obj);
        // Destructor
        ~LightingRig();
        // Initialises lights; for example, loads gobo textures.
        void init();
        // Returns all spot lights contained by this rig.
        std::vector<SpotLight> *getSpotLights();
        // Returns all point lights contained by this rig.
        std::vector<PointLight> *getPointLights();
        // Adds a single spot light to this rig.
        void addLight(SpotLight &s);
        // Adds a single point light to this rig.
        void addLight(PointLight &p);
        // Sets the ambient light intensity.
        void setAmbientIntensity(const float ambientIntensity);
        // Gets the ambient intensity.
        float getAmbientIntensity() const;

    private:
        std::vector<SpotLight> *_spotLights;
        std::vector<PointLight> *_pointLights;
        float _ambientIntensity;
};

} // namespace lighting
} // namespace visualiser

#endif //VISUALISER_LIGHTINGRIG_H
