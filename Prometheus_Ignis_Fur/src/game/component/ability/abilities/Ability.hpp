#pragma once

#include <ecs/component/entity.hpp>

#include "AbilityID.hpp"

struct Ability
{
    virtual ~Ability() = default;
    virtual bool useAbility(ECS::Entity &entity) = 0;

    AbilityID abilityID;
};