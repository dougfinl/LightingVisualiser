//
// Created by dougfinl on 10/11/15.
//

#ifndef VISUALISER_MESH_H
#define VISUALISER_MESH_H

#include "../rendering/IRenderable.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>


namespace visualiser {
namespace stage {

//
// Stores data and manages OpenGL buffers for a single renderable mesh.
//
class Mesh: public rendering::IRenderable {
    public:
        // Returns a shared pointer to the singleton quadrilateral object.
        static std::shared_ptr<Mesh> quad();

        // Creates an empty mesh.
        Mesh();
        // Destructor. Frees OpenGL buffers if allocated.
        virtual ~Mesh();
        // Creates OpenGL buffers and fills with mesh data.
        void init();
        // Renders the mesh given a particular shader program.
        virtual void render(const rendering::ShaderProgram &shader);
        void setVertices(const std::vector<glm::vec3> &vertices);
        void setIndices(const std::vector<unsigned int> &indices);
        void setNormals(const std::vector<glm::vec3> &normals);
        void setTexCoords(const std::vector<glm::vec2> &texCoords);
        void setMaterial(const Material &material);

    private:
        // Singleton quadrilateral.
        static std::shared_ptr<Mesh> QUAD;

        bool _initialised;
        bool _hasMaterial;
        std::vector<glm::vec3> _vertices;
        std::vector<unsigned int> _indices;
        std::vector<glm::vec3> _normals;
        std::vector<glm::vec2> _texCoords;
        GLuint _vao;
        GLuint _vboVertices;
        GLuint _vboNormals;
        GLuint _vboIndices;
        GLuint _vboTexCoords;
        Material _material;
};

} // namespace stage
} // namespace visualiser

#endif //VISUALISER_MESH_H
