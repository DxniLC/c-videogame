#pragma once

#include "TEntity.hpp"

struct SGEngine;
struct ResourceModel;
struct ResourceTexture;
struct TMesh : public TEntity
{
    friend SGEngine;

    explicit TMesh(ResourceModel *model) : mesh(model) {}

    void update() override final;

    void setTexture(ResourceTexture *texture);

private:
    ResourceModel *mesh{nullptr};
};