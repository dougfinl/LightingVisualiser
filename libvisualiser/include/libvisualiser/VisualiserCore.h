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

//
// An interactive, DMX-based 3D lighting visualiser.
//
class VisualiserCore {
    public:
        static VisualiserCore *instance();

        void loadShowFile(const std::string &showFilePath);
        void run() const;

private:
        VisualiserCore();

        void initLogging(const bool debug = false) const;

        static VisualiserCore* _singleInstance;
        std::shared_ptr<stage::Stage> _stage;
        std::shared_ptr<lighting::LightingRig> _rig;
        std::unique_ptr<rendering::Renderer> _renderer;
};

} // namespace visualiser


#endif //VISUALISER_VISUALISER_H
