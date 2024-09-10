#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>
#include <algorithm>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/events/eventsData/ThrowBall.hpp>

struct BTActionChargeAndShoot : BTNode_t
{
    BTActionChargeAndShoot(){}; // Nodo de comportamiento usado para hacer patrones de movimiento definidos

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {

        auto *ia = e.getComponent<AIComponent>();
        auto *ren = e.getComponent<RenderComponent>();
        auto *sensory = e.getComponent<SensoryComponent>();
        auto *entitySensed = entityManager.getEntityByKey(sensory->entityKeySensed);
        auto *sensoredRender = entitySensed->getComponent<RenderComponent>();
        auto throwForce=600.f;
        if (ia && entitySensed)
        {
            auto dir=sensoredRender->node->globalPosition - ren->node->globalPosition; 
            auto orient = glm::normalize(dir);
            if (orient != ren->node->direction && not ren->node->localTransform.interpolated_need_update)
            {
                ren->node->localTransform.setRotateInterpolated(ren->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }


                double chargeAndHit = (ia->charge + ia->nextStep) * ia->chargeRewards[0];

                double chargeAndFail = (ia->charge + ia->nextStep) * ia->chargeRewards[1];

                double shootAndHit = ia->charge * ia->shootRewards[0];

                double shootAndFail = ia->shootRewards[1];

                float percentageHitNextIteration{100};

                auto *entitySensed = entityManager.getEntityByKey(sensory->entityKeySensed);

                auto *renPlayer = entitySensed->getComponent<RenderComponent>();

                auto calc = renPlayer->node->globalPosition - ren->node->globalPosition;

                auto distancia = glm::length(calc);
                if (distancia > ia->ranges[0] && distancia < ia->ranges[1])
                {

                    auto range = ia->ranges[1] - ia->ranges[0];
                    percentageHitNextIteration = 100.0f - ((distancia - ia->ranges[0]) / range) * 100.0f;
                }
                else if (distancia > ia->ranges[1])
                {
                    percentageHitNextIteration = 0;
                }
                if (percentageHitNextIteration == 0)
                {
                    ia->charge = 0;
                    return BTNodeStatus_t::success;
                }
                auto chargePuntuation = percentageHitNextIteration * chargeAndHit - (100 - percentageHitNextIteration) * chargeAndFail;
                auto shootPuntuation = percentageHitNextIteration * shootAndHit + (100 - percentageHitNextIteration) * shootAndFail;

                if (shootPuntuation >= chargePuntuation || ia->charge>=1)
                {

                    auto *eventManager = entityManager.getSingletonComponent<EventManager>();

                    auto *sensoredRender = entitySensed->getComponent<RenderComponent>();
                    auto ballDirection = sensoredRender->node->globalPosition - ren->node->globalPosition;
                    if (eventManager)
                        eventManager->EventQueueInstance.EnqueueLateEvent(ThrowBallData{ren->node->globalPosition, ballDirection, throwForce, ia->charge});
                    ia->charge = 0;
                    ia->haveBall = false;
                    return BTNodeStatus_t::success;
                }
                else
                {
                    std::cout << "cargo\n";
                    ia->charge += ia->nextStep;
                    return BTNodeStatus_t::running;
                }
            
        }

        return BTNodeStatus_t::running;
    }

private:
    glm::vec3 direction{};
};