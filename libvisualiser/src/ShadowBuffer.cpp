//
// Created by dougfinl on 22/03/16.
//

#include "libvisualiser/ShadowBuffer.h"
#include "Constants.h"
#include <boost/log/trivial.hpp>

visualiser::rendering::ShadowBuffer::~ShadowBuffer() {
    if (_texShadowMap > 0) glDeleteTextures(1, &_texShadowMap);
}

bool visualiser::rendering::ShadowBuffer::init(const unsigned int width, const unsigned int height) {
    BOOST_LOG_TRIVIAL(trace) << "Initialising shadow buffer";

    if (!AbstractFramebuffer::init(width, height)) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_texShadowMap);
    glBindTexture(GL_TEXTURE_2D, _texShadowMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texShadowMap, 0);

    // Check for errors
    GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (err != GL_FRAMEBUFFER_COMPLETE) {
        BOOST_LOG_TRIVIAL(error) << "Framebuffer error: " << err;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void visualiser::rendering::ShadowBuffer::bindTextureAttachments() const {
    glActiveTexture(GL_TEXTURE0 + SHADER_SHADOW_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texShadowMap);
}
