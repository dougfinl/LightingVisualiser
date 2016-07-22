//
// Created by dougfinl on 22/03/16.
//

#ifndef VISUALISER_SHADOWBUFFER_H
#define VISUALISER_SHADOWBUFFER_H

#include "AbstractFramebuffer.h"


namespace visualiser {
namespace rendering {

class ShadowBuffer : public AbstractFramebuffer {
    public:
        ShadowBuffer() = default;
        virtual ~ShadowBuffer();

        bool init(const unsigned int width, const unsigned int height) override;

    protected:
        void bindTextureAttachments() const override;

    private:
        GLuint _texShadowMap;
};

} // namespace rendering
} // namespace visualiser


#endif //VISUALISER_SHADOWBUFFER_H
