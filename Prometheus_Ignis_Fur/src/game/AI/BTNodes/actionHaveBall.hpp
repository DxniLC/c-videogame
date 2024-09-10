#pragma once

#include <util/BehaviourTree/node.hpp>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>

struct BTActionHaveBall : BTNode_t
{
    BTActionHaveBall(){} ; // Nodo de comportamiento usado para hacer patrones de movimiento definidos

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir
        auto *ai = e.getComponent<AIComponent>();
        if (ai->haveBall){
            std::cout<<"tengo bola \n";

            return BTNodeStatus_t::fail;

        }
        else{
            std::cout<<"no tengo bola \n";

            return BTNodeStatus_t::success;
            
        }
    }

private:
};