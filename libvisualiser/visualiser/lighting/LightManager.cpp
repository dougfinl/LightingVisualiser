//
// Created by dgf6 on 28/10/15.
//

#include "LightManager.h"
#include "LightingRig.h"
#include "DMXListener.h"
#include <boost/log/trivial.hpp>


visualiser::lighting::LightManager::LightManager(std::shared_ptr<LightingRig> rig, const unsigned int universe, const unsigned int updateIntervalMillis) :
    _running(false),
    _universe(universe),
    _updateIntervalMillis(updateIntervalMillis),
    _rig(rig),
    _thread() {
}

visualiser::lighting::LightManager::~LightManager() {
}

void visualiser::lighting::LightManager::start() {
    if (!_running) {
        _running = true;
        _thread = std::thread(&LightManager::updateLoop, this);
    }
}

void visualiser::lighting::LightManager::stop() {
    if (_running) {
        _running = false;
    }

    if (_thread.joinable()) {
        BOOST_LOG_TRIVIAL(debug) << "Waiting for light manager to finish";
        _thread.join();
    }
}

void visualiser::lighting::LightManager::updateLoop() {
    DMXListener dmxListener(_universe);

    if (dmxListener.Start()) {
        BOOST_LOG_TRIVIAL(debug) << "Started DMX listener";
    } else {
        BOOST_LOG_TRIVIAL(error) << "Could not start DMX listener";
    }

    // Main loop.
    while (_running) {
        auto pointLights = _rig->getPointLights();
        for (auto light = pointLights->begin(); light != pointLights->end(); ++light) {
            light->base.intensity = (float) (dmxListener.getLevel(light->base.address - 1)) / 255.0f;
        }

        auto spotLights = _rig->getSpotLights();
        for (auto light = spotLights->begin(); light != spotLights->end(); ++light) {
            light->base.base.intensity = (float) (dmxListener.getLevel(light->base.base.address - 1)) / 255.0f;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_updateIntervalMillis));
    }

    dmxListener.Stop();
    dmxListener.Join();
}
