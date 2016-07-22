//
// Created by dougfinl on 01/12/15.
//

#ifndef VISUALISER_VISUALISER_H
#define VISUALISER_VISUALISER_H

#include <memory>
#include <string>


namespace visualiser {

namespace stage {

class Stage;

} // namespace stage

namespace lighting {

class LightingRig;

} // namespace lighting

namespace rendering {

class Renderer;

} // namespace rendering

// Stores the configuration for a visualiser program instance.
struct VisualiserConfig {
    std::string showFilePath;
    unsigned int universe;
    unsigned int resX;
    unsigned int resY;
    bool fullscreen;
    bool debugMessages;
    bool realLighting;
};

//
// An interactive, DMX-based 3D lighting visualiser.
//
class Visualiser {
    public:
        // Creates a new visualiser based upon the given configuration.
        Visualiser(const VisualiserConfig &config);
        // Destructor.
        virtual ~Visualiser();
        // Runs the visualiser and displays output to the screen.
        void run() const;

    private:
        void initLogging(const bool debug) const;
        void loadShowFile(const std::string &showFilePath);

        VisualiserConfig _config;
        std::shared_ptr<stage::Stage> _stage;
        std::shared_ptr<lighting::LightingRig> _rig;
        std::unique_ptr<rendering::Renderer> _renderer;
};

} // namespace visualiser


#endif //VISUALISER_VISUALISER_H
