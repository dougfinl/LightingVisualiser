//
// Created by dougfinl on 30/10/15.
//

#ifndef VISUALISER_SHADERPROGRAM_H
#define VISUALISER_SHADERPROGRAM_H

#include <glm/glm.hpp>
#include <string>
#include <GL/glew.h>


namespace visualiser {
namespace rendering {

//
// An OpenGL program that is executed by the GPU.
//
class ShaderProgram {
    public:
        // The types of shader stages that can be used in a shader program.
        enum ShaderType {
            VERTEX,
            FRAGMENT,
            GEOMETRY
        };

        // Returns a textual name of a shader stage type.
        static std::string getShaderTypeString(const ShaderType &type);
        // Creates a new OpenGL shader program.
        ShaderProgram();
        // Destructor
        ~ShaderProgram();
        // Loads a single shader stage from a GLSL source file.
        bool loadShaderFromFile(const std::string &path, const ShaderType &type);
        // Attempt to compile and link the shader program. Returns true if successful, otherwise false.
        bool link();
        // Sets this shader program as the currently active program.
        void use();
        // Overridden method to set a particular uniform in the shader program.
        void setUniform(const GLchar *name, const float val) const;
        void setUniform(const GLchar *name, const int val) const;
        void setUniform(const GLchar *name, const glm::vec3 &v) const;
        void setUniform(const GLchar *name, const glm::mat4 &m) const;

    private:
        void init();
        GLint uniform(const GLchar *name) const;

        bool _initialised;
        bool _inUse;
        GLuint _progHandle;
        GLuint _shaderHandles[3];   // Handles for each type of shader in ShaderProgram::ShaderType
};

} // namespace rendering
} // namespace visualiser

#endif //VISUALISER_SHADERPROGRAM_H
