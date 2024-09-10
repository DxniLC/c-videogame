#pragma once

#include <game/manager/event/EventTrigger.hpp>

#include <game/component/physics.hpp>

struct RepellingForceEvent : public EventTrigger
{
    explicit RepellingForceEvent(CollisionResult &pts, float force) : points(pts), force {force} {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *physics = points.entity_A->template getComponent<PhysicsComponent>();

        if (physics)
        {
            physics->externForces = points.normal * force;
            physics->externForces.y = force;
        }
    }

    CollisionResult points;
    float force;
};