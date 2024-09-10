#pragma once

#include <ecs/component/component.hpp>
#include <memory>

struct Collider;
struct WorldBodyComponent : public ECS::ComponentBase<WorldBodyComponent>
{
    explicit WorldBodyComponent(EntityKey e_id) : ECS::ComponentBase<WorldBodyComponent>(e_id){};

    std::unique_ptr<Collider> collider{nullptr};
    float friction{0.2f};
    float bounciness{0};

    bool IsPassable{false};
};