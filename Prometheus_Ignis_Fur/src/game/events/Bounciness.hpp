
#pragma once

#include <game/manager/event/EventTrigger.hpp>

#include <game/component/body/collider/CollisionResult.hpp>
#include <game/component/body/body.hpp>
#include <game/component/body/scenebody.hpp>

#include <game/component/physics.hpp>

struct BouncinessEvent : public EventTrigger
{
    explicit BouncinessEvent(CollisionResult &pts) : points(pts) {}

    // TODO: Mejorar metodo

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *physics = points.entity_A->getComponent<PhysicsComponent>();
        if (physics)
        {
            auto *bodyWorld = points.entity_B->getComponent<WorldBodyComponent>();
            if (bodyWorld)
                applyBounciness(physics, bodyWorld->bounciness);
            else
            {
                auto *body = points.entity_B->getComponent<BodyComponent>();
                if (body)
                    applyBounciness(physics, body->bounciness);
                else
                {
                    auto *bodyScene = points.entity_B->getComponent<SceneBodyComponent>();
                    if (bodyScene)
                        applyBounciness(physics, bodyScene->bounciness);
                }
            }
        }
    }

    void applyBounciness(PhysicsComponent *physics, float const &bounciness)
    {

        if (points.normal.y < 0.3 && bounciness == 0)
        {
            physics->externForces.x = 0;
            physics->externForces.z = 0;
            physics->isBounce = false;
        }
        else
        {
            if (abs(physics->externForces.y) < 20)
            {
                physics->externForces.y = 0;
                physics->isBounce = false;
            }
            else
            {
                if (bounciness != 0.0f)
                    physics->isBounce = true;
                auto inverseForce = points.normal * glm::length(physics->externForces);

                if (physics->hasInertia)
                {
                    // std::cout << "\nBOUNCINESS: " << bounciness;
                    // std::cout << "\n InverseForce: (" << inverseForce.x << ", " << inverseForce.y << ", " << inverseForce.z;
                    // std::cout << "\n EXTERN: (" << physics->externForces.x << ", " << physics->externForces.y << ", " << physics->externForces.z;
                    // std::cout << "\n----------------------------------------------";
                    physics->externForces += inverseForce * bounciness;
                }
                else
                    physics->externForces = inverseForce * bounciness;
            }
        }
    }

    CollisionResult points;
};