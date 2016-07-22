//
// Created by dougfinl on 21/11/15.
//

#include "LightingRig.h"
#include <boost/log/trivial.hpp>
#include <glm/gtx/rotate_vector.hpp>


visualiser::lighting::LightingRig::LightingRig() :
        _ambientIntensity(0.0f) {
    _spotLights = new std::vector<SpotLight>;
    _pointLights = new std::vector<PointLight>;
}

visualiser::lighting::LightingRig::LightingRig(const LightingRig &obj) {
    _spotLights = new std::vector<SpotLight>;
    *_spotLights = *obj._spotLights;

    _pointLights = new std::vector<PointLight>;
    *_pointLights = *obj._pointLights;
}

visualiser::lighting::LightingRig::~LightingRig() {
    delete _spotLights;
    delete _pointLights;
}

void visualiser::lighting::LightingRig::init() {
    const glm::vec3 WORLD_UP(0.0, 1.0, 0.0);

    // Generate the view-projection matrix for each spot light, and if required, the gobo matrix.
    for (auto &spot : *_spotLights) {
        auto dot = glm::dot(spot.direction, WORLD_UP);
        glm::vec3 lightUp;
        if (dot == 1 || dot == -1) {
            // The light is pointing straight up or straight down.
            lightUp = glm::vec3(0.0, 0.0, 1.0);
        } else {
            lightUp = WORLD_UP;
        }

        // Generate the matrix for the spot light.
        glm::mat4 matLightView = glm::lookAt(spot.base.position, spot.base.position + spot.direction, lightUp);
        glm::mat4 matLightProj = glm::perspective(spot.fieldAngle, 1.0f, 1.0f, 150.0f);
        spot.matrix = matLightProj * matLightView;

        if (spot.hasGobo) {
            spot.gobo->load();

            // Generate the gobo rotation matrix (rotate the entire view about the light's direction).
            glm::vec3 goboUp = glm::rotate(lightUp, spot.goboRotation, spot.direction);
            glm::mat4 matGoboView = glm::lookAt(spot.base.position, spot.base.position + spot.direction, goboUp);
            glm::mat4 matGoboProj = glm::perspective(spot.fieldAngle, 1.0f, 1.0f, 150.0f);
            spot.goboMatrix = matGoboProj * matGoboView;
        }
    }
}

std::vector<visualiser::lighting::SpotLight> *visualiser::lighting::LightingRig::getSpotLights() {
    return _spotLights;
}

std::vector<visualiser::lighting::PointLight> *visualiser::lighting::LightingRig::getPointLights() {
    return _pointLights;
}

void visualiser::lighting::LightingRig::addLight(visualiser::lighting::SpotLight &s) {
    _spotLights->push_back(s);
}

void visualiser::lighting::LightingRig::addLight(visualiser::lighting::PointLight &p) {
    _pointLights->push_back(p);
}

void visualiser::lighting::LightingRig::setAmbientIntensity(const float ambientIntensity) {
    _ambientIntensity = glm::clamp(ambientIntensity, 0.0f, 1.0f);
}

float visualiser::lighting::LightingRig::getAmbientIntensity() const {
    return _ambientIntensity;
}
