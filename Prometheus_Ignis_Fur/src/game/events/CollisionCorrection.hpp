#pragma once

#include <game/manager/event/EventTrigger.hpp>

#include <game/component/body/collider/CollisionResult.hpp>

#include <game/CustomFunctions.hpp>

struct CollisionCorrectionEvent : public EventTrigger
{
    explicit CollisionCorrectionEvent(CollisionResult &pts) : points(pts) {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *render = points.entity_A->template getComponent<RenderComponent>();
        if (render)
        {

            auto position = render->node->localTransform.position;
            if (points.normal.y != 0)
            {
                position.y += points.depth * points.normal.y;
            }
            if (points.normal.y < 0.8)
            {
                if (points.normal.x != 0)
                {

                    position.x += points.depth * points.normal.x;
                }

                if (points.normal.z != 0)
                {
                    position.z += points.depth * points.normal.z;
                }
            }

            render->node->localTransform.setPosition(position);
        }

        // Como raycast tiene update de events, cambia el ground

        // TODO: POR MEJORAR AUN EL REPELING FORCE

        // Force to throw Entity
        // auto *physics = points.entity_A->getComponent<PhysicsComponent>();
        // if (physics)
        // {
        //     if (not physics->isGrounded)
        //     {
        //         glm::vec3 directionForce;
        //         auto *scene = points.entity_B->getComponent<SceneBodyComponent>();

        //         if (scene)
        //         {
        //             directionForce = render->node->localTransform.position - points.polygonGlobalPos;
        //         }
        //         else
        //         {
        //             auto &node = SGFunc::getMainNodeFromEntity(*points.entity_B);
        //             directionForce = render->node->localTransform.position - node.globalPosition;
        //         }
        //         directionForce.y = 0;
        //         directionForce = glm::normalize(directionForce);
        //         physics->externForces += directionForce * 500.f;
        //     }
        // }
    }

private:
    CollisionResult points;
};