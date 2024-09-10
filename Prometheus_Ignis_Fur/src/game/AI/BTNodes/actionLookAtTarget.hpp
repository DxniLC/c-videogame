#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/AI.hpp>

struct BTActionLookAtTarget : BTNode_t
{
    BTActionLookAtTarget(){}; // Nodo de comportamiento usado para hacer patrones de movimiento definidos

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *render = e.getComponent<RenderComponent>();

        auto *ai = e.getComponent<AIComponent>();
        glm::vec3 dir{1, 0, 0};

        if (ai)
            dir = ai->lastSeen - render->node->globalPosition;

        if (render)
        {
            auto orient = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
            if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
            {

                render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }

            return BTNodeStatus_t::success;
        }

        return BTNodeStatus_t::running;
    }

};