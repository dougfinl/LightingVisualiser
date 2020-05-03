//
// Created by dougfinl on 09/02/16.
//

#ifndef VISUALISER_GEOMETRYBUFFER_H
#define VISUALISER_GEOMETRYBUFFER_H

#include "AbstractFramebuffer.h"
#include <GL/glew.h>


namespace visualiser {
namespace rendering {

class GeometryBuffer : public AbstractFramebuffer {
    public:
        GeometryBuffer() = default;
        virtual ~GeometryBuffer();

        bool init(const unsigned int width, const unsigned int height) override;

    protected:
        void bindTextureAttachments() const override;

    private:
        GLuint _texNormal;
        GLuint _texDiffuse;
        GLuint _texSpecular;
        GLuint _texDepth;
};

} // namespace rendering
} // namespace visualiser


#endif //VISUALISER_GEOMETRYBUFFER_H
