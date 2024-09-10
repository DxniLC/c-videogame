
#pragma once

#include <ecs/component/component.hpp>

#include <game/manager/ObjectsTypes.hpp>

#include <unordered_map>

struct ObjectComponent : public ECS::ComponentBase<ObjectComponent>
{
    explicit ObjectComponent(EntityKey e_id) : ECS::ComponentBase<ObjectComponent>(e_id){};

    unsigned int quantity{1};
    ObjectType type{ObjectType::NONE_OBJECT};

};