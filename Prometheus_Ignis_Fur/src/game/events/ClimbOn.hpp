#pragma once

#include <game/manager/event/EventTrigger.hpp>
#include <game/component/body/collider/CollisionResult.hpp>

struct ClimbOnEvent : public EventTrigger
{
    explicit ClimbOnEvent(CollisionResult &pts) : points(pts) {}

    // TODO: Mejorar metodo

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *physics = points.entity_A->template getComponent<PhysicsComponent>();
        if (physics && not physics->isGrounded)
            return;

        if (points.normal.x != 0 || points.normal.z != 0)
        {
            auto *render = points.entity_A->template getComponent<RenderComponent>();
            if (render)
            {
                // Todos lso render son global casi
                auto position = render->node->localTransform.position;

                auto *bodyA = points.entity_A->template getComponent<BodyComponent>();
                if (bodyA)
                {
                    auto size_box_A = bodyA->collider->nodeBox->template getEntity<TBoundingBox>()->size.y / 2.f;
                    auto *bodyB = points.entity_B->template getComponent<WorldBodyComponent>();
                    if (bodyB)
                    {
                        auto size_box_B = bodyB->collider->nodeBox->template getEntity<TBoundingBox>()->size.y / 2.0f;
                        position.y = (bodyB->collider->nodeBox->globalPosition.y + size_box_B) + size_box_A;
                        render->node->setGlobalPosition(position);
                    }
                }
            }
        }
    }

private:
    CollisionResult points;
};