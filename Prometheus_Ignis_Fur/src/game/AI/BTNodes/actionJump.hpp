#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/AI.hpp>
#include <game/component/physics.hpp>

struct BTactionJump : BTNode_t
{
    BTactionJump(int i) { numberJumps = i; };

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *phy = e.getComponent<PhysicsComponent>();
        if (actualJumps == numberJumps)
            return BTNodeStatus_t::success;
        if (phy->isGrounded)
        {
            phy->ownForce.y = 100;
            actualJumps++;
        }
        return BTNodeStatus_t::running;
    }

private:
    int numberJumps{3};
    int actualJumps{0};
};