//
// Created by dougfinl on 23/02/16.
//

#ifndef VISUALISER_BOOSTLOGDESTINATION_H
#define VISUALISER_BOOSTLOGDESTINATION_H

#include <ola/Logging.h>


namespace ola {

class BoostLogDestination : public LogDestination {
    public:
        virtual void Write(log_level level, const std::string &log_line);
};

} // namespace ola


#endif //VISUALISER_BOOSTLOGDESTINATION_H
