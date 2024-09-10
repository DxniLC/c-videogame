#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <game/component/powerup/PowerUpId.hpp>

struct PowerUpData : EventData
{
    explicit PowerUpData(PowerUpId powerUpType)
        : EventData{EventIds::ElementChest}, powerUpType{powerUpType}
    {
    }
    PowerUpId powerUpType;
};