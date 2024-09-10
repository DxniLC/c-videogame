

#pragma once

#include <game/manager/event/EventTrigger.hpp>

#include <game/component/body/collider/CollisionResult.hpp>

#include <game/component/physics.hpp>
#include <game/component/body/body.hpp>
#include <game/component/body/worldbody.hpp>

struct FrictionEvent : public EventTrigger
{
    explicit FrictionEvent(CollisionResult &pts) : points(pts) {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *physics = points.entity_A->getComponent<PhysicsComponent>();
        if (physics && points.normal.y != 0)
        {
            auto *body = points.entity_B->getComponent<WorldBodyComponent>();
            if (body)
            {
                applyFriction(physics->ownForce, body->friction);
                applyFriction(physics->externForces, body->friction);
            }
            else
            {
                auto *body = points.entity_B->getComponent<BodyComponent>();
                if (body)
                {
                    applyFriction(physics->ownForce, body->friction);
                    applyFriction(physics->externForces, body->friction);
                }

                else
                {
                    auto *body = points.entity_B->getComponent<SceneBodyComponent>();
                    if (body)
                    {
                        applyFriction(physics->ownForce, body->friction);
                        applyFriction(physics->externForces, body->friction);
                    }
                }
            }
        }
    }

    void applyFriction(glm::vec3 &force, float friction)
    {
        float fuerzaVector = sqrt((force.x * force.x) + (force.z * force.z));
        if (fuerzaVector <= 40)
        {
            force.x = 0;
            force.z = 0;
        }
        else if (fuerzaVector > 40)
        {
            force.x = force.x * (1 - (friction));
            force.z = force.z * (1 - (friction));
        }
    }

    CollisionResult points;
};