//
// Created by dougfinl on 09/02/16.
//

#include "GeometryBuffer.h"
#include "../Constants.h"
#include <boost/log/trivial.hpp>


visualiser::rendering::GeometryBuffer::~GeometryBuffer() {
    if (_texNormal > 0)   glDeleteTextures(1, &_texNormal);
    if (_texDiffuse > 0)  glDeleteTextures(1, &_texDiffuse);
    if (_texSpecular > 0) glDeleteTextures(1, &_texSpecular);
    if (_texDepth > 0)    glDeleteTextures(1, &_texDepth);
}

bool visualiser::rendering::GeometryBuffer::init(const unsigned int width, const unsigned int height) {
    BOOST_LOG_TRIVIAL(debug) << "Initialising geometry buffer";

    if (!AbstractFramebuffer::init(width, height)) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_texNormal);
    glBindTexture(GL_TEXTURE_2D, _texNormal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texNormal, 0);

    glGenTextures(1, &_texDiffuse);
    glBindTexture(GL_TEXTURE_2D, _texDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _texDiffuse, 0);

    glGenTextures(1, &_texSpecular);
    glBindTexture(GL_TEXTURE_2D, _texSpecular);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _texSpecular, 0);

    glGenTextures(1, &_texDepth);
    glBindTexture(GL_TEXTURE_2D, _texDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texDepth, 0);

    // Draw the buffers to the final framebuffer
    GLuint buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDrawBuffer(GL_DEPTH_ATTACHMENT);

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

void visualiser::rendering::GeometryBuffer::bindTextureAttachments() const {
    glActiveTexture(GL_TEXTURE0 + SHADER_G_NORMAL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texNormal);

    glActiveTexture(GL_TEXTURE0 + SHADER_G_DIFFUSE_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texDiffuse);

    glActiveTexture(GL_TEXTURE0 + SHADER_G_SPECULAR_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texSpecular);

    glActiveTexture(GL_TEXTURE0 + SHADER_G_DEPTH_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, _texDepth);
}
