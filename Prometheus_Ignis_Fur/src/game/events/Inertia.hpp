#pragma once

#include <game/manager/event/EventTrigger.hpp>
#include <game/component/physics.hpp>

struct InertiaEvent : public EventTrigger
{
    explicit InertiaEvent(PhysicsComponent *entity, PhysicsComponent *entity_transport) : entity{entity}, entity_transport{entity_transport} {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {

        // Platform Moving
        if (entity_transport->force.x != 0)
        {
            // Al cambiar hay que tener en cuenta los rozamientos
            if (entity_transport->force.x * entity_transport->ownForce.x < 0)
                entity->externForces.x = entity_transport->ownForce.x * (1.f - 0.035f);
            else
                entity->externForces.x = entity_transport->force.x;
            entity->hasInertia = true;
        }

        // Platform Moving
        if (entity_transport->force.y != 0)
        {
            // Not Jumping
            if (not(entity->externForces.y > entity_transport->force.y))
            {
                if (entity_transport->force.y * entity_transport->ownForce.y < 0)
                    entity->externForces.y = entity_transport->ownForce.y;
                else
                    entity->externForces.y = entity_transport->force.y;
                entity->hasInertia = true;
            }
        }

        // Platform Moving
        if (entity_transport->force.z != 0)
        {
            // Al cambiar hay que tener en cuenta los rozamientos
            if (entity_transport->force.z * entity_transport->ownForce.z < 0)
                entity->externForces.z = entity_transport->ownForce.z * (1.f - 0.035f);
            else
                entity->externForces.z = entity_transport->force.z;
            entity->hasInertia = true;
        }
    }
    PhysicsComponent *entity{nullptr};
    PhysicsComponent *entity_transport{nullptr};
};