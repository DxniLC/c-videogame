#pragma once
#include "node.hpp"
#include <iostream>
#include <vector>
#include <initializer_list>

#include "nodesequence.hpp"

struct BTNodeGroup_t : BTNode_t
{

    using value_type = BTNode_t *;
    using container_type = std::vector<value_type>;
    using init_list = std::initializer_list<value_type>;

    BTNodeGroup_t(init_list l) : nodes{l} {}

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {
        // Ejecutamos todos los hijos en paralelo
        std::string names{};
        bool anyRunning = false;
        for (auto &node : nodes)
        {
            auto status = node->run(e, entityManager);

            switch (status)
            {
            case BTNodeStatus_t::fail:

                for (auto &n : nodes)
                {
                    if (auto seqNode = dynamic_cast<BTNodeSequence_t *>(n))
                    {
                        seqNode->resetSequence();
                    }
                }
                return BTNodeStatus_t::fail;

            case BTNodeStatus_t::running:

                names += std::string(typeid(*node).name()) + " "; // Agregar el nombre del tipo de nodo al std::string nodes
                anyRunning = true;
                break;

            case BTNodeStatus_t::success:

                break;
            }
        }

        this->currentNodeName = names;
        // Si hay alguno en ejecución, entonces la ejecución está en curso
        if (anyRunning)
            return BTNodeStatus_t::running;

        return BTNodeStatus_t::success;
    }

private:
    container_type nodes;
};
