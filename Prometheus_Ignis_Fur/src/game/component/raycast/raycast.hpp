#pragma once

#include <ecs/component/component.hpp>

#include "RaycastBase.hpp"

struct RaycastComponent : public ECS::ComponentBase<RaycastComponent>
{
    explicit RaycastComponent(EntityKey e_id) : ECS::ComponentBase<RaycastComponent>(e_id){rays.reserve(5);};

    std::vector<Raycast> rays{};
    // max raycast to one entity
};