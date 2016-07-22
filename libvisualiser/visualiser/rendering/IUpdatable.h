//
// Created by dougfinl on 29/11/15.
//

#ifndef VISUALISER_IUPDATABLE_H
#define VISUALISER_IUPDATABLE_H

namespace visualiser {
namespace rendering {

//
// Interface for objects that can be updated.
//
class IUpdatable {
    public:
        // Called when an object is required to update its internal values.
        virtual void update() = 0;
};

} // namespace rendering
} // namespace visualiser

#endif //VISUALISER_IUPDATABLE_H
