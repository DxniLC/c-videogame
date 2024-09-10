#pragma once

#include <ecs/component/component.hpp>

#include <engines/graphic/SGEngine/model/Polygon.hpp>

struct SceneBodyComponent : public ECS::ComponentBase<SceneBodyComponent>
{
    explicit SceneBodyComponent(EntityKey e_id) : ECS::ComponentBase<SceneBodyComponent>(e_id){};

    std::vector<Polygon> world_bounds{};
    float friction{0.2f};
    float bounciness{0};

};