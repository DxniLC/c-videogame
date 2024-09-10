#pragma once

#include <util/BehaviourTree/node.hpp>

#include <glm/vec3.hpp>

#include <game/component/physics.hpp>

#include <game/component/render.hpp>

struct BTActionRecollectBall : BTNode_t
{
    BTActionRecollectBall(bool a = true) : ret{a} {}; // Nodo de comportamiento usado para hacer patrones de movimiento definidos

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *phy = e.getComponent<PhysicsComponent>();
        auto *render = e.getComponent<RenderComponent>();
        auto *ai = e.getComponent<AIComponent>();

        if (ai && phy && render)
        {

            glm::vec3 dir = ai->place2HaveBalls - render->node->globalPosition;
            dir.y = 0;
            if (glm::length(dir) < 2)
            {
                ai->haveBall = true;

                if (ret)
                    return BTNodeStatus_t::success;
                else
                    return BTNodeStatus_t::fail;
            }
            auto orient = glm::normalize(dir);
            if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
            {
                render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }

            phy->ownForce += glm::normalize(dir) * 100.0f;
        }
        return BTNodeStatus_t::running;
    }

private:
    bool ret;
};