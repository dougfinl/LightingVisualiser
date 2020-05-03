//
// Created by dougfinl on 26/01/16.
//

#include "libvisualiser/Texture.h"


#define STB_IMAGE_IMPLEMENTATION

#include "../libs/stb_image.h"
#include "Constants.h"
#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>


visualiser::stage::Texture::Texture() :
        _filePath(""),
        _handle(0),
        _type(TEXTURE_NONE),
        _loaded(false) {
}

visualiser::stage::Texture::Texture(const std::string &filePath, const TextureType type) :
        _filePath(filePath),
        _handle(0),
        _type(type),
        _loaded(false) {
}

visualiser::stage::Texture::~Texture() {
    this->unbind();
    if (_handle > 0)
        glDeleteTextures(1, &_handle);
}

bool visualiser::stage::Texture::load() {
    // Prevent the texture from being loaded more than once
    if (_loaded) {
        return false;
    }

    // Don't load the texture if it hasn't been defined with a type
    if (_type == TEXTURE_NONE) {
        return false;
    }

    if (!boost::filesystem::exists(_filePath)) {
        BOOST_LOG_TRIVIAL(warning) << "Texture " << _filePath << " does not exist";
        return false;
    }

    // Load the texture from filePath
    int w, h, bpp;
    unsigned char *img = stbi_load(_filePath.c_str(), &w, &h, &bpp, STBI_rgb);

    // Generate a texture and bind it
    glGenTextures(1, &_handle);
    glBindTexture(GL_TEXTURE_2D, _handle);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // If it's a gobo, create a black border around the image.
    if (_type == TEXTURE_PROJECTED) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColour[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
    }

    // Load the texture onto the graphics card
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free memory allocated to the loaded image
    stbi_image_free(img);

    _loaded = true;

    BOOST_LOG_TRIVIAL(trace) << "Loaded " << textureTypeToString(_type) << " texture " << _filePath << " (" << w <<
                             "x" << h << " pixels)";

    // No error encountered, so return true
    return true;
}

visualiser::stage::Texture::TextureType visualiser::stage::Texture::getType() const {
    return _type;
}

void visualiser::stage::Texture::bind() {
    if (_handle > 0) {
        GLenum activeTexture = GL_TEXTURE0;

        switch (_type) {
            case TEXTURE_DIFFUSE:
                activeTexture = GL_TEXTURE0 + SHADER_MESH_DIFFUSE_TEXTURE;
                break;
            case TEXTURE_SPECULAR:
                activeTexture = GL_TEXTURE0 + SHADER_MESH_SPECULAR_TEXTURE;
                break;
            case TEXTURE_PROJECTED:
                activeTexture = GL_TEXTURE0 + SHADER_GOBO_TEXTURE;
                break;
            default:
                break;
        }

        glActiveTexture(activeTexture);
        glBindTexture(GL_TEXTURE_2D, _handle);
    }
}

void visualiser::stage::Texture::unbind() {
    if (_handle > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

std::string visualiser::stage::Texture::textureTypeToString(const TextureType &type) {
    std::string result;

    switch (type) {
        case TEXTURE_NONE:
            result = "none";
            break;
        case TEXTURE_DIFFUSE:
            result = "diffuse";
            break;
        case TEXTURE_SPECULAR:
            result = "specular";
            break;
        case TEXTURE_PROJECTED:
            result = "projected";
            break;
    }

    return result;
}
