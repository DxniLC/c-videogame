#pragma once

#include <glm/mat4x4.hpp>

struct TEntity
{
    virtual ~TEntity() = default;
    virtual void update() = 0;

    glm::mat4 matrix{1.0f};
    float deltaTime{0.0f};
};
