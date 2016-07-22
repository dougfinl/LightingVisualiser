//
// Created by dgf6 on 28/10/15.
//

#include "Stage.h"
#include <boost/log/trivial.hpp>


visualiser::stage::Stage::Stage() {
}

visualiser::stage::Stage::~Stage() {
}

void visualiser::stage::Stage::addModel(std::unique_ptr<visualiser::stage::Model> model) {
    _models.push_back(std::move(model));
}

std::vector<std::unique_ptr<visualiser::stage::Model>> const& visualiser::stage::Stage::getModels() const {
    return _models;
}
