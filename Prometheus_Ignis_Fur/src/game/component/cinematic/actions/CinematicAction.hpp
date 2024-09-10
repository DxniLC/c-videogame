#pragma once

#include <ecs/component/entity.hpp>

struct CinematicAction
{
    virtual ~CinematicAction() = default;
    virtual bool update(float const &elapsedTime, ECS::Entity *entity, SGEngine *engine, float const & deltaTime) = 0;

    void setTimes(float const &Init, float const &dur)
    {
        InitTime = Init;
        Duration = dur;
    }

    float InitTime{0.0f};
    float Duration{0.0f};
};