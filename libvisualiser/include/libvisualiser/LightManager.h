//
// Created by dgf6 on 28/10/15.
//

#ifndef VISUALISER_LIGHTCONTROLLER_H
#define VISUALISER_LIGHTCONTROLLER_H

#include "IUpdatable.h"
#include <memory>
#include <thread>


namespace visualiser {
namespace lighting {

class LightingRig;
class DMXListener;

//
// Uses a DMX listener to update the intensities of lights based on data from a particular universe.
//
class LightManager {
    public:
        // Creates a light manager to update the lights that lie on the specified universe in the specified lighting
        // rig.
        explicit LightManager(std::shared_ptr<LightingRig> rig, const unsigned int universe, const unsigned int updateIntevalMillis);
        // Destructor.
        virtual ~LightManager();
        void start();
        void stop();

    private:
        // Repetedly updates the lighting rig with new values from the DMX listener.
        void updateLoop();

        bool _running;
        unsigned int _universe;
        unsigned int _updateIntervalMillis;
        std::shared_ptr<LightingRig> _rig;
        std::thread _thread;
};

} // namespace lighting
} // namespace visualiser

#endif //VISUALISER_LIGHTCONTROLLER_H
