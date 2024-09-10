#pragma once

#include <ecs/component/component.hpp>

struct Blackboard : ECS::SingletonComponentBase<Blackboard>
{
    explicit Blackboard() = default;

    std::vector<std::vector<int>> matriz;

    EntityKey seenKey;
    glm::vec3 PlayerTargetPosition{};
    bool someCalculatedPath{false};
    glm::vec3 AIAdvertPosition{};
    float rangeAdvert{100.0f};

    bool targetAction{false};
};