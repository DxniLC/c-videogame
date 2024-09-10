
#pragma once

#include <ecs/component/component.hpp>

struct EventComponent : public ECS::ComponentBase<EventComponent>
{
    explicit EventComponent(EntityKey e_id) : ECS::ComponentBase<EventComponent>(e_id){};

    std::string eventName{""};
    int ID{0};
    float time{0.0f};
    bool Processed{false};
};