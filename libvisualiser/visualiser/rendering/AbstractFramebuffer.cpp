//
// Created by dougfinl on 22/03/16.
//

#include "AbstractFramebuffer.h"
#include <boost/log/trivial.hpp>

visualiser::rendering::AbstractFramebuffer::~AbstractFramebuffer() {
    if (_fbo > 0) glDeleteFramebuffers(1, &_fbo);
}

bool visualiser::rendering::AbstractFramebuffer::init(const unsigned int width, const unsigned int height) {
    if (!GLEW_ARB_framebuffer_object) {
        BOOST_LOG_TRIVIAL(error) << "OpenGL framebuffer objects are not supported";
        return false;
    }

    glGenFramebuffers(1, &_fbo);

    return true;
}

void visualiser::rendering::AbstractFramebuffer::bind(const bool writeEnable) const {
    if (writeEnable) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    } else {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
        bindTextureAttachments();
    }
}

