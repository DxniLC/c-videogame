#pragma once

#include "TEntity.hpp"

struct SGEngine;
struct ResourceModel;
struct TWindEffect : public TEntity
{
    friend SGEngine;

    explicit TWindEffect(ResourceModel *model, glm::vec2 direction, float windForce) : mesh(model), WindDirection{direction}, WindForce{windForce} {}

    void update() override final;

private:
    ResourceModel *mesh{nullptr};
    glm::vec2 WindDirection{1, 0};
    float WindForce{0.10f};
    float WindTime{0.0f};

    const double LoopAngle{ 2 * 3.14159265358979323846 };
};