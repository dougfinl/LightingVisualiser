//
// Created by dougfinl on 18/11/15.
//

#ifndef VISUALISER_DMXLISTENER_H
#define VISUALISER_DMXLISTENER_H

#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/client/ClientWrapper.h>
#include <ola/thread/Thread.h>
#include <ola/StringUtils.h>


namespace visualiser {
namespace lighting {

//
// Connects with the Open Lighting Architecture and receives data for a single universe.
//
class DMXListener: public ola::thread::Thread {
    public:
        // Creates a DMX listener for the specified universe.
        explicit DMXListener(const unsigned int universe);
        // Destructor
        virtual ~DMXListener();
        // Registers the listener with the Open Lighting Architecture, and starts listening for data.
        virtual bool Start();
        // Terminates the DMX listener.
        virtual void Stop();
        // Retrieves the value for a single address on the registered universe.
        uint8_t getLevel(unsigned short address) const;

    private:
        virtual void *Run();
        void onUniverseName(const ola::client::Result &result);
        void onUniverseRegister(const ola::client::Result &result);
        void onDMXReceived(const ola::client::DMXMetadata &metadata, const ola::DmxBuffer &data);

        unsigned int _universe;
        uint8_t *_universeData;
        ola::client::OlaClientWrapper *_wrapper;
        ola::client::OlaClient *_client;
        ola::LogDestination *_logDestination;
};

} // namespace lighting
} // namespace visualiser

#endif //VISUALISER_DMXLISTENER_H
