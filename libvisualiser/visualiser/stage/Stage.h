//
// Created by dgf6 on 28/10/15.
//

#ifndef VISUALISER_STAGE_H
#define VISUALISER_STAGE_H

#include "Model.h"
#include <memory>
#include <vector>


namespace visualiser {
namespace stage {

//
// Stores a collection of models.
//
class Stage {
    public:
        // Create an empty stage.
        Stage();
        // Copy constructor.
        Stage(const Stage &obj) = delete;
        // Assignment operator.
        Stage &operator=(const Stage &obj) = delete;
        // Destructor.
        ~Stage();
        // Adds a model to the stage. Stage takes ownership of the model.
        void addModel(std::unique_ptr<visualiser::stage::Model> model);
        // Returns all models contained by this stage.
        std::vector<std::unique_ptr<Model>> const& getModels() const;

    private:
        std::vector<std::unique_ptr<Model>> _models;
};

} // namespace stage
} // namespace visualiser

#endif //VISUALISER_STAGE_H
