
#pragma once
#include <ecs/component/component.hpp>

#include <engines/graphic/SGEngine/HUD/elements/HUDElement.hpp>

struct ResourceTexture;
struct RenderMenuComponent : public ECS::ComponentBase<RenderMenuComponent>
{

    explicit RenderMenuComponent(EntityKey e_id) : ECS::ComponentBase<RenderMenuComponent>(e_id){};

    HUDElement *hudElement{nullptr};

};
