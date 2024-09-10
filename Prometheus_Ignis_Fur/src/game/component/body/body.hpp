#pragma once

#include <ecs/component/component.hpp>

#include <game/component/body/collider/AABB.hpp>
#include <game/component/body/collider/OBB.hpp>

#include <memory>

struct Collider;
struct BodyComponent : public ECS::ComponentBase<BodyComponent>
{
    explicit BodyComponent(EntityKey e_id) : ECS::ComponentBase<BodyComponent>(e_id){};
    
    std::unique_ptr<Collider> collider{nullptr};
    // Transform transform;
    float friction{0.2f};
    float bounciness{0};
    bool canCollide{true};

    bool IsPassable{false};
};
