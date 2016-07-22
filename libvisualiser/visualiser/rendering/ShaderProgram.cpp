//
// Created by dougfinl on 30/10/15.
//

#include "ShaderProgram.h"
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <boost/log/trivial.hpp>


std::string visualiser::rendering::ShaderProgram::getShaderTypeString(const ShaderType &type) {
    std::string str = "";

    switch (type) {
        case VERTEX:
            str = "Vertex";
            break;
        case FRAGMENT:
            str = "Fragment";
            break;
        case GEOMETRY:
            str = "Geometry";
            break;
        default:
            str = "Unknown";
            break;
    }

    return str;
}

visualiser::rendering::ShaderProgram::ShaderProgram() :
    _initialised(false),
    _inUse(false) {
    for (int i = 0; i < 4; ++i) {
        _shaderHandles[i] = 0;
    }
}

visualiser::rendering::ShaderProgram::~ShaderProgram() {
    if (_progHandle > 0)
        glDeleteProgram(_progHandle); // Note: this program's shaders are also deleted with this call
}

bool visualiser::rendering::ShaderProgram::loadShaderFromFile(const std::string &path, const ShaderType &type) {
    if (!_initialised)
        init();

    bool success = false;

    std::ifstream fIn;
    fIn.open(path);
    if (fIn.is_open()) {
        // Read the shader source into a single string
        std::string shaderString;
        shaderString.assign(std::istreambuf_iterator<char>(fIn), std::istreambuf_iterator<char>());
        const GLchar *shader = shaderString.c_str();

        if (_shaderHandles[type] != 0) {
            BOOST_LOG_TRIVIAL(error) << getShaderTypeString(type) << " already loaded in this program";
            return false;
        }

        // Create the shader
        switch (type) {
            case VERTEX:
                _shaderHandles[VERTEX] = glCreateShader(GL_VERTEX_SHADER);
                break;
            case FRAGMENT:
                _shaderHandles[FRAGMENT] = glCreateShader(GL_FRAGMENT_SHADER);
                break;
            case GEOMETRY:
                _shaderHandles[GEOMETRY] = glCreateShader(GL_GEOMETRY_SHADER);
                break;
        }

        // Load & compile the shader, and attach it to this program
        glShaderSource(_shaderHandles[type], 1, &shader, NULL);
        glCompileShader(_shaderHandles[type]);
        glAttachShader(_progHandle, _shaderHandles[type]);

        // Check for errors
        GLint status;
        glGetShaderiv(_shaderHandles[type], GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            BOOST_LOG_TRIVIAL(error) << "Unable to compile shader " << _shaderHandles[type] << " from " << path;

            // Retrieve errors and print to stderr
            GLint infoLogLength = 0;
            glGetShaderiv(_shaderHandles[type], GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(_shaderHandles[type], infoLogLength, &infoLogLength, &infoLog[0]);
            std::string errString = "";
            for (auto c : infoLog) {
                errString += c;
            }

            BOOST_LOG_TRIVIAL(error) << errString;

            glDeleteShader(_shaderHandles[type]);
        } else {
            success = true;
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "Unable to open " << path << " for reading";
    }

    BOOST_LOG_TRIVIAL(debug) << getShaderTypeString(type) << " shader loaded from " << path;

    return success;
}

bool visualiser::rendering::ShaderProgram::link() {
    bool success = false;

    glLinkProgram(_progHandle);

    // Check for errors
    GLint linkStatus;
    glGetProgramiv(_progHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        BOOST_LOG_TRIVIAL(error) << "Failed to link shader program " << _progHandle;

        // Retrieve errors and print to stderr
        GLint infoLogLength = 0;
        glGetProgramiv(_progHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(_progHandle, infoLogLength, &infoLogLength, &infoLog[0]);
        std::string errString = "";
        for (auto c : infoLog) {
            errString += c;
        }

        BOOST_LOG_TRIVIAL(error) << errString;
    } else {
        success = true;

        // Detach the shaders from the program
        for (auto shaderHandle : _shaderHandles) {
            glDetachShader(_progHandle, shaderHandle);
        }

        BOOST_LOG_TRIVIAL(debug) << "Shader program " << _progHandle << " linked successfully";
    }

    return success;
}

void visualiser::rendering::ShaderProgram::use() {
    glUseProgram(_progHandle);
    _inUse = true;
}

void visualiser::rendering::ShaderProgram::setUniform(const GLchar *name, const float v0) const {
    GLint loc = uniform(name);
    glUniform1f(loc, v0);
}

void visualiser::rendering::ShaderProgram::setUniform(const GLchar *name, const int v0) const {
    GLint loc = uniform(name);
    glUniform1i(loc, v0);
}

void visualiser::rendering::ShaderProgram::setUniform(const GLchar *name, const glm::vec3 &v) const {
    GLint loc = uniform(name);
    glUniform3f(loc, v.x, v.y, v.z);
}

void visualiser::rendering::ShaderProgram::setUniform(const GLchar *name, const glm::mat4 &m) const {
    GLint loc = uniform(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

GLint visualiser::rendering::ShaderProgram::uniform(const GLchar *name) const {
    return glGetUniformLocation(_progHandle, name);
}

void visualiser::rendering::ShaderProgram::init() {
    _progHandle = glCreateProgram();
    _initialised = true;
}
