//
// Created by dougfinl on 10/11/15.
//

#include "libvisualiser/Mesh.h"
#include "Constants.h"
#include <boost/log/trivial.hpp>


std::shared_ptr<visualiser::stage::Mesh> visualiser::stage::Mesh::QUAD = nullptr;

std::shared_ptr<visualiser::stage::Mesh> visualiser::stage::Mesh::quad() {
    // If the quad doesn't yet exist...
    if (!Mesh::QUAD) {
        Mesh::QUAD = std::make_shared<Mesh>();
        Mesh::QUAD->setVertices({
                           glm::vec3(-1.0f, -1.0f, 0.0f),
                           glm::vec3(-1.0f, 1.0f, 0.0f),
                           glm::vec3(1.0f, -1.0f, 0.0f),
                           glm::vec3(1.0f, 1.0f, 0.0)
                       });
        Mesh::QUAD->setIndices({0, 2, 1, 1, 2, 3});
        Mesh::QUAD->setNormals({
                          glm::vec3(0.0f, 0.0f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f)
                      });
        Mesh::QUAD->setTexCoords({
                            glm::vec2(0.0f, 0.0f),
                            glm::vec2(0.0f, 1.0f),
                            glm::vec2(1.0f, 0.0f),
                            glm::vec2(1.0f, 1.0f)
                        });
        Mesh::QUAD->init();
    }

    return Mesh::QUAD;
}

visualiser::stage::Mesh::Mesh() :
    _initialised(false),
    _hasMaterial(false)
{
}

visualiser::stage::Mesh::~Mesh() {
    if (_vboVertices > 0)  glDeleteBuffers(1, &_vboVertices);
    if (_vboNormals > 0)   glDeleteBuffers(1, &_vboNormals);
    if (_vboTexCoords > 0) glDeleteBuffers(1, &_vboTexCoords);
    if (_vboIndices > 0)   glDeleteBuffers(1, &_vboIndices);
    if (_vao > 0)          glDeleteVertexArrays(1, &_vao);
}

void visualiser::stage::Mesh::init() {
    // Prevents initialising the mesh more than once
    if (_initialised) {
        return;
    }

    if (_vertices.empty()) {
        BOOST_LOG_TRIVIAL(warning) << "Attempting to initialise mesh with no vertices";
        return;
    }

    if (_indices.empty()) {
        BOOST_LOG_TRIVIAL(warning) << "Attempting to initialise mesh with no indices";
        return;
    }

    if (_normals.empty()) {
        BOOST_LOG_TRIVIAL(warning) << "Attempting to initialise mesh with no normals";
        return;
    }

    // Create and bind vertex array object
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Store vertices in video memory
    glGenBuffers(1, &_vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, _vboVertices);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), _vertices.data(), GL_STATIC_DRAW);

    // Create a pointer to vertex data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(SHADER_POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Store normals in video memory
    glGenBuffers(1, &_vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
    glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(glm::vec3), _normals.data(), GL_STATIC_DRAW);

    // Create a pointer to normal data
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(SHADER_NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Store texture coordinates in video memory
    glGenBuffers(1, &_vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, _vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size() * sizeof(glm::vec2), _texCoords.data(), GL_STATIC_DRAW);

    // Create a pointer to texture coordinate data
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(SHADER_TEXCOORD_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Store indices in video memory
    glGenBuffers(1, &_vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load the required textures from file
    if (_hasMaterial) {
        if (_material.hasTexDiffuse)
            _material.texDiffuse.load();

        if (_material.hasTexSpecular)
            _material.texSpecular.load();
    }

    _initialised = true;
}

void visualiser::stage::Mesh::render(const rendering::ShaderProgram &shader) {
    if (_hasMaterial) {
        shader.setUniform("mat.colourDiffuse", _material.colourDiffuse);
        shader.setUniform("mat.colourSpecular", _material.colourSpecular);

        if (_material.hasTexDiffuse) {
            shader.setUniform("mat.hasTexDiffuse", true);
            shader.setUniform("texDiffuse", SHADER_MESH_DIFFUSE_TEXTURE);
            _material.texDiffuse.bind();
        } else {
            shader.setUniform("mat.hasTexDiffuse", false);
        }

        if (_material.hasTexSpecular) {
            shader.setUniform("mat.hasTexSpecular", true);
            shader.setUniform("texSpecular", SHADER_MESH_SPECULAR_TEXTURE);
            _material.texSpecular.bind();
        } else {
            shader.setUniform("mat.hasTexSpecular", false);
        }
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void visualiser::stage::Mesh::setVertices(const std::vector<glm::vec3> &vertices) {
    if (_initialised) {
        BOOST_LOG_TRIVIAL(warning) << "Can't change vertices of already initialised mesh";
        return;
    }

    _vertices = vertices;
}

void visualiser::stage::Mesh::setIndices(const std::vector<unsigned int> &indices) {
    if (_initialised) {
        BOOST_LOG_TRIVIAL(warning) << "Can't change indices of already initialised mesh";
        return;
    }

    _indices = indices;
}

void visualiser::stage::Mesh::setNormals(const std::vector<glm::vec3> &normals) {
    if (_initialised) {
        BOOST_LOG_TRIVIAL(warning) << "Can't change normals of already initialised mesh";
        return;
    }

    _normals = normals;
}

void visualiser::stage::Mesh::setTexCoords(const std::vector<glm::vec2> &texCoords) {
    if (_initialised) {
        BOOST_LOG_TRIVIAL(warning) << "Can't change texture coordinates of already initialised mesh";
        return;
    }

    _texCoords = texCoords;
}

void visualiser::stage::Mesh::setMaterial(const visualiser::stage::Material &material) {
    if (_initialised) {
        BOOST_LOG_TRIVIAL(warning) << "Can't change material of already initialised mesh";
        return;
    }

    _material = material;
    _hasMaterial = true;
}
