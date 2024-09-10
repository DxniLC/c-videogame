#pragma once

#include "TEntity.hpp"

#include <vector>

struct SGEngine;
struct ResourceModel;
struct ResourceTexture;
struct TSceneMesh : public TEntity
{
    friend SGEngine;

    explicit TSceneMesh() = default;

    explicit TSceneMesh(std::vector<ResourceModel *> &models)
    {
        this->models = models;
    }

    void addModel(ResourceModel *model);

    void update() override final;

private:
    std::vector<ResourceModel *> models;
    
};