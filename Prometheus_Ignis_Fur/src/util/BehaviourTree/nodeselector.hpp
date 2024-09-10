#pragma once
#include "node.hpp"
#include <iostream>
#include <vector>
#include <typeinfo>
#include <initializer_list>


struct BTNodeSelector : BTNode_t{
    
    using value_type = BTNode_t*;
    using container_type = std::vector<value_type>;
    using init_list = std::initializer_list<value_type>;
    using iterator = std::vector<value_type>::iterator;

    BTNodeSelector(init_list l) : nodes{l} {}

    void resetSequence() {currentNodeIt = nodes.begin();}

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final{
    
    //Mirar si estoy en el nodo final
        if(currentNodeIt == nodes.end())
            resetSequence();

    //Ejecutar el nodo y obtengo el status
        auto status = (*currentNodeIt)->run(e, entityManager);
        
        switch(status)
        {
            case BTNodeStatus_t::success: { resetSequence(); return BTNodeStatus_t::success; }
            case BTNodeStatus_t::running: 
            {
                std::string name = typeid(**currentNodeIt).name();
                if (name.size() > 2 && std::isdigit(name[0]) && name[1] == ' ')
                {
                    name = name.substr(2); // Remove the leading number and space
                    std::cout << "ENTRO AQUI: " << name;
                }
                if(name!="BTNodeGroup_t" || name!="BTNodeSequence_t")
                {
                    
                    this->currentNodeName = name;
                }
                    

                 return BTNodeStatus_t::running; 
            }
            case BTNodeStatus_t::fail: 
            {
                ++currentNodeIt;
                if(currentNodeIt == nodes.end())
                {
                    resetSequence();

                    return BTNodeStatus_t::fail;

                }

            }

        }

        
        return BTNodeStatus_t::running;

    }

    private:
        
        container_type nodes;
        iterator  currentNodeIt { nodes.begin() };
        
       
};