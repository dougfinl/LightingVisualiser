//
// Created by dougfinl on 21/11/15.
//

#include "libvisualiser/ShowFileLoader.h"
#include "libvisualiser/Stage.h"
#include "libvisualiser/LightingRig.h"
#include "libvisualiser/ColourManager.h"
#include "Constants.h"
#include "libvisualiser/Camera.h"
#include <fstream>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <glm/gtx/string_cast.hpp>


visualiser::ShowFileLoader::ShowFileLoader() :
        _loadedStage(std::make_shared<stage::Stage>()),
        _loadedRig(std::make_shared<lighting::LightingRig>()),
        _colourManager(std::make_unique<ColourManager>(GEL_COLOUR_FILE_PATH)) {
}

visualiser::ShowFileLoader::~ShowFileLoader() {
    BOOST_LOG_TRIVIAL(trace) << "Deleting show file loader";
}

bool visualiser::ShowFileLoader::load(const std::string &filePath) {
    std::ifstream fin(filePath);
    if (!fin) {
        BOOST_LOG_TRIVIAL(error) << "File " << filePath << " does not exist";
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "Loading show file " << filePath;

    try {
        YAML::Node showNode = YAML::Load(fin);

        // Load models
        YAML::Node modelsNode = showNode["models"];
        if (modelsNode) {
            BOOST_LOG_TRIVIAL(info) << "Loading models...";
            if (!modelsNode.IsSequence()) {
                BOOST_LOG_TRIVIAL(error) << "\"models\" section must be formatted as a YAML list";
                return false;
            }
            unsigned short loadedCount = 0;
            size_t size = modelsNode.size();
            for (size_t i = 0; i < size; i++) {
                if (loadModel(modelsNode[i]))
                    ++loadedCount;
            }
            BOOST_LOG_TRIVIAL(info) << loadedCount << " loaded, " << size - loadedCount << " failed";
        } else {
            BOOST_LOG_TRIVIAL(info) << "No models found";
        }

        // Load spot lights
        YAML::Node spotLightsNode = showNode["spotLights"];
        if (spotLightsNode) {
            BOOST_LOG_TRIVIAL(info) << "Loading spot lights...";
            if (!spotLightsNode.IsSequence()) {
                BOOST_LOG_TRIVIAL(error) << "\"spotLights\" section must be formatted as a YAML list";
                return false;
            }
            unsigned short loadedCount = 0;
            size_t size = spotLightsNode.size();
            for (size_t i = 0; i < size; i++) {
                if (loadSpotLight(spotLightsNode[i]))
                    ++loadedCount;
            }
            BOOST_LOG_TRIVIAL(info) << loadedCount << " loaded, " << size - loadedCount << " failed";
        } else {
            BOOST_LOG_TRIVIAL(info) << "No spot lights found";
        }

        // Load point lights
        YAML::Node pointLightsNode = showNode["pointLights"];
        if (pointLightsNode) {
            BOOST_LOG_TRIVIAL(info) << "Loading point lights...";
            if (!pointLightsNode.IsSequence()) {
                BOOST_LOG_TRIVIAL(error) << "\"pointLights\" section must be formatted as a YAML list";
                return false;
            }
            unsigned short loadedCount = 0;
            size_t size = pointLightsNode.size();
            for (size_t i = 0; i < size; i++) {
                if (loadPointLight(pointLightsNode[i]))
                    ++loadedCount;
            }
            BOOST_LOG_TRIVIAL(info) << loadedCount << " loaded, " << size - loadedCount << " failed";
        } else {
            BOOST_LOG_TRIVIAL(info) << "No point lights found";
        }

        if (showNode["ambientIntensity"]) {
            float ambientIntensity = showNode["ambientIntensity"].as<float>();
            _loadedRig->setAmbientIntensity(ambientIntensity);
            BOOST_LOG_TRIVIAL(debug) << "Set ambient intensity to " << ambientIntensity*100 << "%";
        } else {
            _loadedRig->setAmbientIntensity(DEFAULT_AMBIENT_INTENSITY);
            BOOST_LOG_TRIVIAL(debug) << "Using default ambient intensity of " << DEFAULT_AMBIENT_INTENSITY*100 << "%";
        }

        YAML::Node cameraNode = showNode["camera"];
        if (cameraNode) {
            YAML::Node presetsNode = cameraNode["presets"];
            if (!presetsNode.IsSequence()) {
                BOOST_LOG_TRIVIAL(error) << "Camera \"presets\" section must be formatted as a YAML list";
                return false;
            }
            unsigned short loadedCount = 0;
            size_t size = presetsNode.size();
            for (size_t i = 0; i < size; i++) {
                if (loadCameraPreset(presetsNode[i]))
                    ++loadedCount;
            }
            BOOST_LOG_TRIVIAL(info) << loadedCount << " camera presets loaded, " << size - loadedCount << " failed";
        } else {
            BOOST_LOG_TRIVIAL(info) << "No camera presets found";
        }
    } catch (YAML::ParserException &e) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to parse show file: " << e.msg;
        return false;
    }

    return true;
}

std::shared_ptr<visualiser::stage::Stage> visualiser::ShowFileLoader::getStage() const {
    return _loadedStage;
}

std::shared_ptr<visualiser::lighting::LightingRig> visualiser::ShowFileLoader::getLightingRig() const {
    return _loadedRig;
}

std::map<unsigned short, visualiser::rendering::CameraPreset> visualiser::ShowFileLoader::getCameraPresets() const {
    return _cameraPresets;
}

bool visualiser::ShowFileLoader::loadPointLight(const YAML::Node &n) {
    lighting::PointLight p;

    if (n["address"]) {
        p.base.address = n["address"].as < unsigned
        short > ();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Light declared without address";
        return false;
    }

    unsigned int cTemp = 3000;
    if (n["ctemp"]) {
        cTemp = n["ctemp"].as < unsigned
        int > ();
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << p.base.address << ") has no colour temperature;\tusing 3000K";
    }

    if (n["gel"]) {
        p.base.colour = _colourManager->lightTosRGB(cTemp, n["gel"].as<std::string>());
    } else {
        p.base.colour = _colourManager->lightTosRGB(cTemp);
    }

    p.base.intensity = 0.0f;

    if (n["position"]) {
        YAML::Node positionNode = n["position"];
        p.position.x = positionNode[0].as<float>();
        p.position.y = positionNode[1].as<float>();
        p.position.z = positionNode[2].as<float>();
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << p.base.address << ") has no position;\tusing (0,0,0)";
        p.position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    _loadedRig->addLight(p);

    return true;
}

// TODO: this should really make use of loadPointLight
bool visualiser::ShowFileLoader::loadSpotLight(const YAML::Node &n) {
    lighting::SpotLight s;
    if (n["address"]) {
        s.base.base.address = n["address"].as < unsigned
        short > ();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Light declared without address";
        return false;
    }

    unsigned int cTemp = 3000;
    if (n["ctemp"]) {
        cTemp = n["ctemp"].as < unsigned
        int > ();
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << s.base.base.address << ") has no colour temperature;\tusing 3000K";
    }

    if (n["gel"]) {
        s.base.base.colour = _colourManager->lightTosRGB(cTemp, n["gel"].as<std::string>());
    } else {
        s.base.base.colour = _colourManager->lightTosRGB(cTemp);
    }

    s.base.base.intensity = 0.0f;

    if (n["position"]) {
        YAML::Node positionNode = n["position"];
        s.base.position.x = positionNode[0].as<float>();
        s.base.position.y = positionNode[1].as<float>();
        s.base.position.z = positionNode[2].as<float>();
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << s.base.base.address << ") has no position;\tusing (0,0,0)";
        s.base.position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if (n["beamAngle"]) {
        s.beamAngle = glm::radians(n["beamAngle"].as<float>());
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << s.base.base.address << ") has no beam angle;\tusing 26 degrees";
        s.beamAngle = glm::radians(26.0f);
    }

    // If no field angle is defined, assume the spot light has a hard edge
    if (n["fieldAngle"]) {
        s.fieldAngle = glm::radians(n["fieldAngle"].as<float>());
    } else {
        s.fieldAngle = s.beamAngle;
    }

    if (n["direction"] && n["target"]) {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << s.base.base.address
                                   << ") must be declared with either a direction or a target";
        return false;
    }

    if (n["direction"]) {
        YAML::Node directionNode = n["direction"];
        s.direction.x = directionNode[0].as<float>();
        s.direction.y = directionNode[1].as<float>();
        s.direction.z = directionNode[2].as<float>();
    } else if (n["target"]) {
        // Calculate the light's direction from its position and target
        YAML::Node targetNode = n["target"];
        glm::vec3 target;
        target.x = targetNode[0].as<float>();
        target.y = targetNode[1].as<float>();
        target.z = targetNode[2].as<float>();

        s.direction = target - s.base.position;
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Light (#" << s.base.base.address
                                   << ") has no direction or target;\tpointing downwards";
        s.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    }
    s.direction = glm::normalize(s.direction);

    s.hasGobo = false;
    if (n["gobo"]) {
        std::string goboPath = GOBO_SEARCH_PATH + "/" + n["gobo"].as<std::string>() + ".png";

        if (boost::filesystem::exists(goboPath)) {
            s.hasGobo = true;
            s.gobo = new stage::Texture(goboPath, stage::Texture::TEXTURE_PROJECTED);
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Gobo " << goboPath << " could not be found";
        }

        if (n["goboRotation"]) {
            float rotationDeg = n["goboRotation"].as<float>();
            s.goboRotation = glm::radians(rotationDeg);
        } else {
            s.goboRotation = 0.0f;
        }
    }

    _loadedRig->addLight(s);

    return true;
}

bool visualiser::ShowFileLoader::loadModel(const YAML::Node &n) {
    std::string modelSrc;
    if (n["src"]) {
        modelSrc = n["src"].as<std::string>();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Model declared without a source path";
        return false;
    }

    if (!boost::filesystem::exists(modelSrc)) {
        BOOST_LOG_TRIVIAL(error) << modelSrc << " not found";
        return false;
    }

    auto m(stage::loadModelFromFile(modelSrc));

    if (m == nullptr) {
        return false;
    }

    if (n["position"]) {
        YAML::Node positionNode = n["position"];
        glm::vec3 position(positionNode[0].as<float>(), positionNode[1].as<float>(), positionNode[2].as<float>());
        m->translate(position);
    }

    if (n["rotation"]) {
        YAML::Node rotationNode = n["rotation"];
        glm::vec3 rotation(rotationNode[0].as<float>(), rotationNode[1].as<float>(), rotationNode[2].as<float>());
        m->rotate(rotation);
    }

    if (n["scale"]) {
        float scale = n["scale"].as<float>();
        m->scale(scale);
    }

    // Stage takes ownership of the model.
    _loadedStage->addModel(std::move(m));

    return true;
}

bool visualiser::ShowFileLoader::loadCameraPreset(const YAML::Node &n) {
    unsigned short presetKey;
    rendering::CameraPreset preset;

    YAML::Node keyNode = n["key"];
    if (!keyNode) {
        BOOST_LOG_TRIVIAL(error) << "Camera preset defined without a key";
        return false;
    }

    YAML::Node positionNode = n["position"];
    if (!positionNode) {
        BOOST_LOG_TRIVIAL(error) << "Camera preset " << presetKey << " defined without a position";
        return false;
    }

    YAML::Node targetNode = n["target"];
    if (!targetNode) {
        BOOST_LOG_TRIVIAL(error) << "Camera preset " << presetKey << " defined without a target";
        return false;
    }

    presetKey = keyNode.as<unsigned short>();
    preset.position = glm::vec3(positionNode[0].as<float>(), positionNode[1].as<float>(), positionNode[2].as<float>());
    preset.target = glm::vec3(targetNode[0].as<float>(), targetNode[1].as<float>(), targetNode[2].as<float>());

    if (_cameraPresets.count(presetKey) > 0) {
        BOOST_LOG_TRIVIAL(warning) << "Camera preset " << presetKey << " already loaded; overwriting";
    }

    _cameraPresets[presetKey] = preset;

    return true;
}
