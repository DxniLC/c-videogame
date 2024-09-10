#pragma once

#include <ecs/component/entity.hpp>
#include "../PowerUpId.hpp"

struct PowerUp
{
    virtual ~PowerUp() = default;

    void usePowerUp(ECS::Entity &entity)
    {
        Actived = true;
        use(entity);
    }

    bool updatePowerUp(ECS::Entity &entity)
    {
        if (Actived)
            return update(entity);
        return false;
    }

    PowerUpId powerID;

protected:
    bool Actived{false};
    virtual void use(ECS::Entity &entity) = 0;
    virtual bool update(ECS::Entity &entity) = 0;
};