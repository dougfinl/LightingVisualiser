//
// Created by dougfinl on 18/11/15.
//

#include "libvisualiser/DMXListener.h"
#include "BoostLogDestination.h"
#include <boost/log/trivial.hpp>


visualiser::lighting::DMXListener::DMXListener(const unsigned int universe) : ola::thread::Thread(),
                                                                              _universe(universe) {
    _universeData = new uint8_t[512];
    _logDestination = new ola::BoostLogDestination;

    // Fill the universe data array with zeros
    for (int i = 0; i < 512; i++) {
        _universeData[i] = 0;
    }
}

visualiser::lighting::DMXListener::~DMXListener() {
    delete _client;
    delete[] _universeData;
    delete _logDestination;
}

bool visualiser::lighting::DMXListener::Start() {
    BOOST_LOG_TRIVIAL(info) << "Initialising OLA";
    ola::InitLogging(ola::OLA_LOG_MAX, _logDestination);

    _wrapper = new ola::client::OlaClientWrapper;
    if (!_wrapper->Setup()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialise OLA";
        exit(1);
    }

    _client = _wrapper->GetClient();
    _client->RegisterUniverse(_universe,
                              ola::client::REGISTER,
                              ola::NewSingleCallback(this, &DMXListener::onUniverseRegister));
    std::stringstream universeName;
    universeName << "CO3015 Lighting Visualiser Input (Universe " << _universe << ")";
    _client->SetUniverseName(_universe, universeName.str(), ola::NewSingleCallback(this, &DMXListener::onUniverseName));
    _client->SetDMXCallback(ola::NewCallback(this, &DMXListener::onDMXReceived));

    return ola::thread::Thread::Start();
}

void visualiser::lighting::DMXListener::Stop() {
    BOOST_LOG_TRIVIAL(info) << "Terminating OLA";
    _wrapper->GetSelectServer()->Terminate();
}

uint8_t visualiser::lighting::DMXListener::getLevel(unsigned short address) const {
    return _universeData[address];
}

void visualiser::lighting::DMXListener::onUniverseName(const ola::client::Result &result) {
    if (result.Success()) {
        BOOST_LOG_TRIVIAL(trace) << "Updated name of universe " << _universe;
    } else {
        BOOST_LOG_TRIVIAL(error) << "Failed to change name of universe " << _universe << ": " << result.Error();
    }
}

void visualiser::lighting::DMXListener::onUniverseRegister(const ola::client::Result &result) {
    if (result.Success()) {
        BOOST_LOG_TRIVIAL(info) << "Registered DMX listener on universe " << _universe;
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Failed to register DMX listener for universe " << _universe << ": "
                                   << result.Error();
    }
}

void visualiser::lighting::DMXListener::onDMXReceived(const ola::client::DMXMetadata &metadata,
                                                      const ola::DmxBuffer &data) {
    unsigned int length;
    data.Get(_universeData, &length);
}

void *visualiser::lighting::DMXListener::Run() {
    _wrapper->GetSelectServer()->Run();
    return nullptr;
}

