//
// Created by dougfinl on 21/03/16.
//

#include "libvisualiser/PostProcessBuffer.h"
#include "Constants.h"
#include <boost/log/trivial.hpp>


visualiser::rendering::PostProcessBuffer::~PostProcessBuffer() {
    if (_texColour > 0) glDeleteTextures(1, &_texColour);
}

bool visualiser::rendering::PostProcessBuffer::init(const unsigned int width, const unsigned int height) {
    BOOST_LOG_TRIVIAL(trace) << "Initialising post-process buffer";

    if (!AbstractFramebuffer::init(width, height)) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_texColour);
    glBindTexture(GL_TEXTURE_2D, _texColour);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texColour, 0);

    // Draw the colour texture to the framebuffer.
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

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

void visualiser::rendering::PostProcessBuffer::bindTextureAttachments() const {
    glActiveTexture(GL_TEXTURE0 + SHADER_POSTPROCESS_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texColour);
}
