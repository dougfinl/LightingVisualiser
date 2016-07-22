//
// Created by dougfinl on 26/01/16.
//

#ifndef VISUALISER_TEXTURE_H
#define VISUALISER_TEXTURE_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>


namespace visualiser {
namespace stage {

//
// Manages the loading and use of a texture belonging to a mesh.
//
class Texture {
    public:
        enum TextureType {
            TEXTURE_NONE,
            TEXTURE_DIFFUSE,
            TEXTURE_SPECULAR,
            TEXTURE_PROJECTED
        };

        static std::string textureTypeToString(const TextureType &type);

        // Creates an empty texture.
        Texture();
        // Creates a specific type of texture based on an image file.
        explicit Texture(const std::string &filePath, const TextureType type);
        // Destructor.
        virtual ~Texture();
        // Loads the texture from file, creating the necessary OpenGL buffers. Returns true if successful, otherwise
        // false.
        bool load();
        // Returns the type of the texture.
        TextureType getType() const;
        // Binds the texture for rendering.
        void bind();
        // Unbinds the texture from the graphics card.
        void unbind();

    private:
        std::string _filePath;
        GLuint _handle;
        TextureType _type;
        bool _loaded;
};

struct Material {
    glm::vec3 colourDiffuse;
    bool hasTexDiffuse;
    Texture texDiffuse;
    glm::vec3 colourSpecular;
    bool hasTexSpecular;
    Texture texSpecular;
};

} // namespace stage
} // namespace visualiser

#endif //VISUALISER_TEXTURE_H
