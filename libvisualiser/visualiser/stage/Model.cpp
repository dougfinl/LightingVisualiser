//
// Created by dougfinl on 10/11/15.
//

#include "Model.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../Constants.h"
#include "../../make_unique.h"


visualiser::stage::Model::Model() {
    _translation = glm::mat4(1.0f); // Identity matrix (no transformation)
    _rotation = glm::mat4(1.0f);
    _scale = glm::mat4(1.0f);
}

visualiser::stage::Model::Model(const std::vector<Mesh*> &meshes) :
        _meshes(meshes) {
    _translation = glm::mat4(1.0f); // Identity matrix (no transformation)
    _rotation = glm::mat4(1.0f);
    _scale = glm::mat4(1.0f);
}

visualiser::stage::Model::~Model() {
    for (auto it = _meshes.begin(); it != _meshes.end(); it++) {
        delete (*it);
    }
    _meshes.clear();
}

void visualiser::stage::Model::init() {
    for (auto mesh : _meshes) {
        mesh->init();
    }
}

void visualiser::stage::Model::render(const rendering::ShaderProgram &shader) {
    glm::mat4 transformation = getTransformation();

    // Send the transformation matrix to the shader
    glm::vec4 col1 = transformation[0];
    glm::vec4 col2 = transformation[1];
    glm::vec4 col3 = transformation[2];
    glm::vec4 col4 = transformation[3];
    glVertexAttrib4fv(SHADER_MODEL_TRANSFORMATION_ATTRIBUTE, glm::value_ptr(col1));
    glVertexAttrib4fv(SHADER_MODEL_TRANSFORMATION_ATTRIBUTE + 1, glm::value_ptr(col2));
    glVertexAttrib4fv(SHADER_MODEL_TRANSFORMATION_ATTRIBUTE + 2, glm::value_ptr(col3));
    glVertexAttrib4fv(SHADER_MODEL_TRANSFORMATION_ATTRIBUTE + 3, glm::value_ptr(col4));

    for (auto mesh : _meshes) {
        mesh->render(shader);
    }
}

void visualiser::stage::Model::rotate(const glm::vec3 &rot) {
    _rotation = glm::rotate(_rotation, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    _rotation = glm::rotate(_rotation, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _rotation = glm::rotate(_rotation, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void visualiser::stage::Model::translate(const glm::vec3 &trans) {
    _translation = glm::translate(_translation, trans);
}

void visualiser::stage::Model::scale(const float &factor) {
    _scale = glm::scale(_scale, glm::vec3(factor));
}

glm::mat4 visualiser::stage::Model::getTransformation() const {
    return _translation * _rotation * _scale;
}

// Utility functions
std::unique_ptr<visualiser::stage::Model> visualiser::stage::loadModelFromFile(const std::string &path) {
    std::vector<Mesh*> meshes;

    if (!path.empty()) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

        // Get the parent directory of the model
        const std::string directory = boost::filesystem::path(path).parent_path().string();

        // Check for import errors
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            BOOST_LOG_TRIVIAL(error) << "Error importing model " << path << " (" << importer.GetErrorString() << ")";
            return nullptr;
        }

        // Load each mesh
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];

            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> texCoords;
            if (mesh->mNumVertices > 0) {
                for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                    // Vertex
                    const aiVector3D &vec = mesh->mVertices[v];
                    vertices.push_back(glm::vec3(vec.x, vec.y, vec.z));

                    // Normal
                    const aiVector3D &nor = mesh->mNormals[v];
                    normals.push_back(glm::vec3(nor.x, nor.y, nor.z));

                    // Texture coordinate
                    if (mesh->HasTextureCoords(0)) {
                        const aiVector3D &coord = mesh->mTextureCoords[0][v];
                        texCoords.push_back(glm::vec2(coord.x, coord.y));
                    }
                }
            }

            std::vector<unsigned int> indices;
            for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
                const aiFace *face = &mesh->mFaces[f];
                // Each triangular face should have exactly 3 indices
                if (face->mNumIndices != 3) {
                    BOOST_LOG_TRIVIAL(warning) << "Found a face with " << face->mNumIndices << " indices; skipping";
                    continue;
                }
                indices.push_back(face->mIndices[0]);
                indices.push_back(face->mIndices[1]);
                indices.push_back(face->mIndices[2]);
            }

            // Check if the mesh contains a material; if so, load it.
            Material meshMaterial;
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

                // Load the diffuse colour.
                aiColor3D colDiffuse(0.0f, 0.0f, 0.0f);
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, colDiffuse);
                meshMaterial.colourDiffuse = glm::vec3(colDiffuse.r, colDiffuse.g, colDiffuse.b);

                // Load the diffuse texture.
                if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                    std::string texturePath = "";

                    // Only load one texture of the specified type.
                    aiString path;
                    // TODO: check return code for error?
                    mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

                    texturePath.append(directory);
                    texturePath.append("/");
                    texturePath.append(path.C_Str());

                    meshMaterial.texDiffuse = Texture(texturePath.c_str(), Texture::TEXTURE_DIFFUSE);
                    meshMaterial.hasTexDiffuse = true;
                } else {
                    meshMaterial.hasTexDiffuse = false;
                }

                // Load the specular colour.
                aiColor3D colSpecular(0.0f, 0.0f, 0.0f);
                mat->Get(AI_MATKEY_COLOR_SPECULAR, colSpecular);
                meshMaterial.colourSpecular = glm::vec3(colSpecular.r, colSpecular.g, colSpecular.b);

                // Load the specular texture.
                if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
                    std::string texturePath = "";

                    // Only load one texture of the specified type.
                    aiString path;
                    // TODO: check return code for error?
                    mat->GetTexture(aiTextureType_SPECULAR, 0, &path);

                    texturePath.append(directory);
                    texturePath.append("/");
                    texturePath.append(path.C_Str());

                    BOOST_LOG_TRIVIAL(trace) << "SPECULAR TEXTURE: " << texturePath;

                    meshMaterial.texSpecular = Texture(texturePath.c_str(), Texture::TEXTURE_SPECULAR);
                    meshMaterial.hasTexSpecular = true;
                } else {
                    meshMaterial.hasTexSpecular = false;
                }
            }

            Mesh *m = new Mesh;
            m->setVertices(vertices);
            m->setIndices(indices);
            m->setNormals(normals);
            m->setTexCoords(texCoords);
            m->setMaterial(meshMaterial);

            meshes.push_back(m);
        }
    }

    return std::make_unique<Model>(meshes);
}
