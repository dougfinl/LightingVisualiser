//
// Created by dougfinl on 29/11/15.
//

#ifndef VISUALISER_IRENDERABLE_H
#define VISUALISER_IRENDERABLE_H

#include "ShaderProgram.h"


namespace visualiser {
namespace rendering {

//
// Interface for objects that can be rendererd.
//
class IRenderable {
    public:
        // Called when an object is required to be rendered.
        virtual void render(const ShaderProgram &shader) = 0;
};

} // namespace rendering
} // namespace visualiser

#endif //VISUALISER_IRENDERABLE_H
