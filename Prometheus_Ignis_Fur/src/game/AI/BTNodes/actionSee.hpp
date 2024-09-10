#pragma once
#include <util/BehaviourTree/node.hpp>
#include <chrono>
#include <iostream>
#include <game/AI/Blackboard.hpp>
#include <game/component/sensory.hpp>
#include <game/events/eventsData/BBTargetData.hpp>
#include <game/component/sensory.hpp>

struct BTActionSee_t : BTNode_t
{

    BTActionSee_t() = default;

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *render = e.getComponent<RenderComponent>();

        if (render)
        {

            auto *ai = e.getComponent<AIComponent>();
            auto *sensory = e.getComponent<SensoryComponent>();

            if (sensory->iSee)
            {
                auto *entitySensed = entityManager.getEntityByKey(sensory->entityKeySensed);
                auto *renderSensed = entitySensed->getComponent<RenderComponent>();
                ai->lastSeen = renderSensed->node->globalPosition;

                std::cout << render->node->globalPosition.x << "-" << render->node->globalPosition.y << "-" << render->node->globalPosition.z << "mi antes oisucsjdnadsadsadbsagtdayhj \n";
                if (ai->haveIBack)
                    ai->lastPlaceISee = render->node->globalPosition;
                ai->haveIBack=false;
                auto *blackboard = entityManager.template getSingletonComponent<Blackboard>();
                if (blackboard)
                {
                    blackboard->seenKey = sensory->entityKeySensed;
                    blackboard->PlayerTargetPosition = renderSensed->node->globalPosition;
                    blackboard->AIAdvertPosition = render->node->globalPosition;
                    blackboard->rangeAdvert = ai->rangeToAdvise;
                    auto *eventManager = entityManager.getSingletonComponent<EventManager>();
                    if (eventManager)
                        eventManager->EventQueueInstance.EnqueueLateEvent(BBTargetData{blackboard});
                }

                return BTNodeStatus_t::fail;
            }
            else
            {
                return BTNodeStatus_t::success;
            }
        }

        return BTNodeStatus_t::running;
    }
};