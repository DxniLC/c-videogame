
#pragma once

#include <ecs/component/component.hpp>

#include <unordered_map>

#include <memory>

#include "powerups/MoreJump.hpp"
#include "powerups/MoreSpeed.hpp"

struct PowerUpComponent : public ECS::ComponentBase<PowerUpComponent>
{
    explicit PowerUpComponent(EntityKey e_id) : ECS::ComponentBase<PowerUpComponent>(e_id){};

    std::vector<std::unique_ptr<PowerUp>> powerUps{};
    int PowerUpIndex{0};
    bool IsUsingPowerUp{false};
};