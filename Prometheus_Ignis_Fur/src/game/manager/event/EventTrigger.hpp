#pragma once

#include <ecs/manager/entitymanager.hpp>

struct EventTrigger
{
    virtual ~EventTrigger() = default;
    virtual void handle(ECS::EntityManager &entityMan) = 0;
};
