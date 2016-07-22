//
// Created by dougfinl on 21/03/16.
//

#ifndef VISUALISER_POSTPROCESSBUFFER_H
#define VISUALISER_POSTPROCESSBUFFER_H

#include "AbstractFramebuffer.h"
#include <GL/glew.h>


namespace visualiser {
namespace rendering {

class PostProcessBuffer : public AbstractFramebuffer {
    public:
        PostProcessBuffer() = default;
        virtual ~PostProcessBuffer();

        bool init(const unsigned int width, const unsigned int height) override;

    protected:
        void bindTextureAttachments() const override;

    private:
        GLuint _texColour;
};

} // namespace rendering
} // namespace visualiser


#endif //VISUALISER_POSTPROCESSBUFFER_H
