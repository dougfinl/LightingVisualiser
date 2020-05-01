//
// Created by dougfinl on 01/12/15.
//

#include "Visualiser.h"
#include "ShowFileLoader.h"
#include "rendering/Renderer.h"
#include "stage/Stage.h"
#include "lighting/LightManager.h"
#include <boost/log/utility/setup.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>


namespace logging  = boost::log;
namespace expr     = logging::expressions;
namespace keywords = logging::keywords;

visualiser::Visualiser::Visualiser(const visualiser::VisualiserConfig &config) :
    _config(config),
    _renderer(std::make_unique<rendering::Renderer>(config.resX, config.resY, config.fullscreen)) {
    initLogging(config.debugMessages);

    loadShowFile(config.showFilePath);
    if (config.realLighting)
        _renderer->setLightingMode(rendering::Renderer::REALISTIC_LIGHTING);
}

visualiser::Visualiser::~Visualiser() {
}

void visualiser::Visualiser::run() const {
    lighting::LightManager lightManager(_rig, _config.universe, 20);

    lightManager.start();
    _renderer->start();
    lightManager.stop();
}

void visualiser::Visualiser::initLogging(const bool debug) const {
    logging::add_console_log(
        std::cout,
        keywords::format = (
            expr::stream << "["
                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                << "] "
                << logging::trivial::severity
                << "\t> "
                << expr::smessage
        ),
        keywords::auto_flush = true
    );
    logging::add_common_attributes();
    if (debug) {
        logging::core::get()->set_filter(
            logging::trivial::severity >= logging::trivial::trace
        );
    } else {
        logging::core::get()->set_filter(
            logging::trivial::severity >= logging::trivial::info
        );
    }
}

void visualiser::Visualiser::loadShowFile(const std::string &showFilePath) {
    ShowFileLoader loader;
    if (loader.load(showFilePath)) {
        _stage = loader.getStage();
        _rig = loader.getLightingRig();

        _renderer->setStage(_stage);
        _renderer->setLightingRig(_rig);
        _renderer->setCameraPresets(loader.getCameraPresets());
    }
}
