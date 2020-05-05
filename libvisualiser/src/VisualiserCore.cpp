//
// Created by dougfinl on 01/12/15.
//

#include "libvisualiser/VisualiserCore.h"
#include "libvisualiser/ShowFileLoader.h"
#include "libvisualiser/Renderer.h"
#include "libvisualiser/Stage.h"
#include "libvisualiser/LightManager.h"
#include <boost/log/utility/setup.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>


namespace logging  = boost::log;
namespace expr     = logging::expressions;
namespace keywords = logging::keywords;

visualiser::VisualiserCore* visualiser::VisualiserCore::_singleInstance = nullptr;

visualiser::VisualiserCore::VisualiserCore() :
    _renderer(std::make_unique<rendering::Renderer>()) {
    initLogging();
}

void visualiser::VisualiserCore::run() const {
    // FIXME currently running on universe 1 by default
    lighting::LightManager lightManager(_rig, 1, 20);

    lightManager.start();
    _renderer->start();
    lightManager.stop();
}

void visualiser::VisualiserCore::initLogging(const bool debug) const {
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

void visualiser::VisualiserCore::loadShowFile(const std::string &showFilePath) {
    ShowFileLoader loader;
    if (loader.load(showFilePath)) {
        _stage = loader.getStage();
        _rig = loader.getLightingRig();

        _renderer->setStage(_stage);
        _renderer->setLightingRig(_rig);
        _renderer->setCameraPresets(loader.getCameraPresets());
    }
}

visualiser::VisualiserCore *visualiser::VisualiserCore::instance() {
    if (!_singleInstance) {
        _singleInstance = new VisualiserCore();
    }

    return _singleInstance;
}
