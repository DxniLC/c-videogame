#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/physics.hpp>

#include <game/component/AI.hpp>

struct BTActionMoveLastSeen : BTNode_t
{
    BTActionMoveLastSeen(){};

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {

        auto *phy = e.getComponent<PhysicsComponent>();
        auto *render = e.getComponent<RenderComponent>();
        auto *ai = e.getComponent<AIComponent>();
        if (!ai->haveIBack)
        {
            if (phy && render)
            {
                
                glm::vec3 dir = ai->lastPlaceISee - render->node->globalPosition;
                dir.y = 0;
                if (glm::length(dir) < 0.5)
                {
                    ai->haveIBack = true;
                    return BTNodeStatus_t::success;
                }
                auto orient = glm::normalize(dir);
                if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
                {
                    render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
                }
                phy->ownForce += glm::normalize(dir) * 100.0f;
            }
        }
        return BTNodeStatus_t::running;
    }
};