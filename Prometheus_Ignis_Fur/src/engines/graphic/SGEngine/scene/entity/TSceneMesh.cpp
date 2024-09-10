
#include "TSceneMesh.hpp"

#include <engines/graphic/resource/ResourceModel.hpp>

#include <glm/mat4x4.hpp>

#include <iostream>

void TSceneMesh::update()
{
    for (auto &model : models)
        model->draw(matrix);
}

void TSceneMesh::addModel(ResourceModel *model)
{
    models.emplace_back(model);
}