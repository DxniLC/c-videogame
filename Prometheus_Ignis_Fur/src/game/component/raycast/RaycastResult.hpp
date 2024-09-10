#pragma once

#include <ecs/component/entity.hpp>
#include <game/manager/rules/FunctionIDs.hpp>

struct RaycastResult
{
    explicit RaycastResult() = default;
    explicit RaycastResult(float distance_pts) : distance(distance_pts) {}

    glm::vec3 direction{};
    ECS::Entity *entity_A{nullptr};
    ECS::Entity *entity_B{nullptr};
    FunctionID functionID{FunctionID::NONEFUNC};
    float distance{__FLT_MAX__};
    glm::vec3 intersection{};

    bool has_intersection{false};
};