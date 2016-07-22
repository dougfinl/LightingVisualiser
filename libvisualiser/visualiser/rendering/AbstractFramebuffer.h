//
// Created by dougfinl on 22/03/16.
//

#ifndef VISUALISER_ABSTRACTFRAMEBUFFER_H
#define VISUALISER_ABSTRACTFRAMEBUFFER_H

#include <GL/glew.h>


namespace visualiser {
namespace rendering {

class AbstractFramebuffer {
    public:
        virtual ~AbstractFramebuffer();

        virtual bool init(const unsigned int width, const unsigned int height);
        void bind(const bool writeEnable = false) const;

    protected:
        GLuint _fbo;

        virtual void bindTextureAttachments() const = 0;
};

} // namespace rendering
} // namespace visualiser


#endif //VISUALISER_ABSTRACTFRAMEBUFFER_H
