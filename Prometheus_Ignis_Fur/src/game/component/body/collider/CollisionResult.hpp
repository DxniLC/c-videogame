
#pragma once

#include <game/manager/rules/FunctionIDs.hpp>

#include <glm/vec3.hpp>

namespace ECS
{
    struct Entity;
};

struct CollisionResult
{
    explicit CollisionResult() = default;
    ECS::Entity *entity_A{nullptr};
    ECS::Entity *entity_B{nullptr};
    glm::vec3 normal{};
    float depth{0};
    bool hasCollision{false};
    FunctionID functionID{FunctionID::NONEFUNC};

    // Scene Bodies
    glm::vec3 polygonGlobalPos{};
};