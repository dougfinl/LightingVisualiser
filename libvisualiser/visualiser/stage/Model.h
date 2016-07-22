//
// Created by dougfinl on 10/11/15.
//

#ifndef VISUALISER_MODEL_H
#define VISUALISER_MODEL_H


#include "../rendering/IRenderable.h"
#include "Mesh.h"
#include "Texture.h"
#include <memory>
#include <string>


namespace visualiser {
namespace stage {

//
// Stores a transformed collection of meshes.
//
class Model: public rendering::IRenderable {
    public:
        // Creates an empty model.
        Model();
        // Creates a model with the specified meshes.
        Model(const std::vector<Mesh*> &meshes);
        // Destructor.
        virtual ~Model();
        // Loads the model from the previous specified model file.
        void init();
        // Renders the model given a specific shader program.
        virtual void render(const rendering::ShaderProgram &shader);
        // Rotates the model by a number of degrees.
        void rotate(const glm::vec3 &rot);
        // Transforms the model.
        void translate(const glm::vec3 &trans);
        // Scales the model equally along all three axes.
        void scale(const float &factor);

    private:
        glm::mat4 getTransformation() const;

        glm::mat4 _translation;
        glm::mat4 _rotation;
        glm::mat4 _scale;
        std::vector<Mesh *> _meshes;
};

// Utility functions
std::unique_ptr<visualiser::stage::Model> loadModelFromFile(const std::string &path);

} // namespace stage
} // namespace visualiser

#endif //VISUALISER_MODEL_H
