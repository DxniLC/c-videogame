#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/AI.hpp>
#include <game/component/physics.hpp>

struct BTActionPatroll_t : BTNode_t
{
    BTActionPatroll_t(){};

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *render = e.getComponent<RenderComponent>();
        auto *ai = e.getComponent<AIComponent>();
        auto *phy = e.getComponent<PhysicsComponent>();

        if (render && ai)
        {

            glm::vec3 direction = ai->patrollPoints[position] - render->node->globalPosition;
            
            direction.y = 0.f;

            auto orient = glm::normalize(direction);
            if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
            {
                render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }
            

            phy->ownForce += orient * 50.f;


            if (glm::length(direction) < 0.2)
            {
                position++;
                if (position == int(ai->patrollPoints.size()))
                {

                    position = 0;
                }
            }
            return BTNodeStatus_t::running;
        }
        return BTNodeStatus_t::fail;
    }

private:
    int position{0};
};