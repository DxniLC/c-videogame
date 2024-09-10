#pragma once
#include "node.hpp"
#include <iostream>


struct BTDecisionAlternative_t : BTNode_t{
    BTDecisionAlternative_t(bool a = false) : alternative{a} {}

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final{      //override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir
        
        alternative = !alternative;
        if(alternative) return BTNodeStatus_t::success;
        return BTNodeStatus_t::fail;
    }

    private:
        bool alternative {false};
};