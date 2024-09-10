
#pragma once
#include <util/BehaviourTree/node.hpp>
#include <chrono>
#include <iostream>

struct BTActionAdviseHearing_t : BTNode_t
{

    BTActionAdviseHearing_t() = default;

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {
        auto *blackboard = entityManager.template getSingletonComponent<Blackboard>();
        auto *render = e.getComponent<RenderComponent>();
        auto *ai = e.getComponent<AIComponent>();
        if (blackboard && render && ai)
        {
            if (blackboard->targetAction)
            {
                auto const &currentPos = render->node->globalPosition;
                auto distance = glm::length(blackboard->AIAdvertPosition - currentPos);

                if (distance <= blackboard->rangeAdvert)
                {

                    ai->lastSeen = blackboard->PlayerTargetPosition;
                    return BTNodeStatus_t::fail;
                }
            }
        }
        return BTNodeStatus_t::success;
    }
};