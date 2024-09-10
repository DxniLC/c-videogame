
#pragma once

#include <ecs/component/component.hpp>

#include <functional>

struct InputMenuComponent : public ECS::ComponentBase<InputMenuComponent>
{

    explicit InputMenuComponent(EntityKey e_id) : ECS::ComponentBase<InputMenuComponent>(e_id){};

    bool do_action{false};
    std::function<void()> actionFunc{nullptr};
    bool isHovered{false};
    bool isClicked{false};
};
