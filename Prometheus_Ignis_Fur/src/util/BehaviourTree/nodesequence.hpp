#pragma once
#include "node.hpp"
#include <iostream>
#include <vector>
#include <initializer_list>


struct BTNodeSequence_t : BTNode_t{
    
    using value_type = BTNode_t*;
    using container_type = std::vector<value_type>;
    using init_list = std::initializer_list<value_type>;
    using iterator = std::vector<value_type>::iterator;

    BTNodeSequence_t(init_list l) : nodes{l} {}

    void resetSequence() {currentNodeIt = nodes.begin();}

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final{      //override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir
    
    //Mirar si estoy en el nodo final
        if(currentNodeIt == nodes.end())
            resetSequence();

    //Ejecutar el nodo y obtengo el status
        auto status = (*currentNodeIt)->run(e, entityManager);
        
        switch(status)
        {
            case BTNodeStatus_t::fail: { resetSequence(); return BTNodeStatus_t::fail; }
            case BTNodeStatus_t::running: 
            {
                this->currentNodeName = typeid(**currentNodeIt).name();
                 return BTNodeStatus_t::running; 
            }
            case BTNodeStatus_t::success: 
            {
                ++currentNodeIt;
                if(currentNodeIt == nodes.end())
                {
                    resetSequence();

                    return BTNodeStatus_t::success;

                }

            }

        }

        
        return BTNodeStatus_t::running;

    }

    private:
        
        container_type nodes;
        iterator  currentNodeIt { nodes.begin() };
        
       
};