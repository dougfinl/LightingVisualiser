//
// Created by dougfinl on 21/11/15.
//

#ifndef VISUALISER_SHOWFILELOADER_H
#define VISUALISER_SHOWFILELOADER_H

#include "Camera.h"
#include <map>
#include <string>
#include <yaml-cpp/node/node.h>


namespace visualiser {

namespace stage {

class Stage;

}

namespace lighting {

class LightingRig;

}

class ColourManager;

//
// Extracts a stage and lighitng rig from a show file.
//
class ShowFileLoader {
    public:
        // Constructor.
        ShowFileLoader();
        // Destructor.
        ~ShowFileLoader();
        // Loads a show file. Returns true if successful, otherwise false.
        bool load(const std::string &filePath);
        // Returns the stage loaded from a show file. If no show file is loaded, nullptr is returned.
        std::shared_ptr<stage::Stage> getStage() const;
        // Returns the lighting rig from a show file. If no show file is loaded, nullptr is returned.
        std::shared_ptr<lighting::LightingRig> getLightingRig() const;
        // Returns the camera presets loaded from the show file. If no show file is loaded, an empty map is returned.
        std::map<unsigned short, rendering::CameraPreset> getCameraPresets() const;

    private:
        bool loadPointLight(const YAML::Node &n);
        bool loadSpotLight(const YAML::Node &n);
        bool loadModel(const YAML::Node &n);
        bool loadCameraPreset(const YAML::Node &n);

        std::shared_ptr<stage::Stage> _loadedStage;
        std::shared_ptr<lighting::LightingRig> _loadedRig;
        std::unique_ptr<ColourManager> _colourManager;
        std::map<unsigned short, rendering::CameraPreset> _cameraPresets;
};

} // namespace visualiser

#endif //VISUALISER_SHOWFILELOADER_H
