#pragma once

#include <ecs/component/component.hpp>

#include <engines/graphic/SGEngine/HUD/elements/HUDImage.hpp>

#include <chrono>

struct ResourceTexture;
struct AnimationMenuComponent : public ECS::ComponentBase<AnimationMenuComponent>
{

    explicit AnimationMenuComponent(EntityKey e_id) : ECS::ComponentBase<AnimationMenuComponent>(e_id){};

    HUDImage *animationElement{nullptr};

    // bool for inverse
    std::function<void(AnimationMenuComponent &, bool)> HoverAnimation{nullptr};
    std::function<void(AnimationMenuComponent &)> ClickAnimation{nullptr};

    bool startHoverAnimation{false};
    bool startClickAnimation{false};
};