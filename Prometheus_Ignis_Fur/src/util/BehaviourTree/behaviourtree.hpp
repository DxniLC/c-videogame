#pragma once
#include <vector>
#include <memory>
#include <concepts>
#include "node.hpp"

template <typename T>
concept BTNodeTypye = std::derived_from<T, BTNode_t>; // el tipo T cumplira el concepto de ser un tipo nodo de behaviour_tree si T es derivado de la clase behaviour_tree

struct BehaviourTree_t
{ // nunca poner un destructor custom salvo que lo vayamos a poner y sea necesario, nunca nucna por que sino c++ cuenta como que nosotros lo quermos gestioanr y él no lo hara
    using value_type = std::unique_ptr<BTNode_t, BTNode_t::Deleter>;
    using NodeStorage_t = std::vector<value_type>;
    using MemoryStorage_t = std::unique_ptr<std::byte[]>;

    explicit BehaviourTree_t() {}

    BTNodeStatus_t run(ECS::Entity &&e, ECS::EntityManager &&entityManager) noexcept { return run(e, entityManager); };
    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept
    {
        if (nodes.size() > 0)
        {

            nodes.back()->run(e, entityManager); // los nodos se crean de abajo a arriba, el último nodo que se mete en el arbo les el nod oraíz
            this->currentnode = nodes.back()->currentNodeName;
            // comienza ejecutando el último nodo qeu es el nodo raiz ( será el priemro en la memoria reservada )
        };
        return BTNodeStatus_t::fail;
    }

    template <BTNodeTypye NodeType, typename... ParamTypes>
    NodeType &createNode(ParamTypes &&...params)
    {
        // Reserev memory
        ptr_reserved -= sizeof(NodeType); // Dir de memoria dodne creará el nuevo nodo - dir de memoria total menos l oque ocupa el nodo pedido
        if (ptr_reserved < mem.get())
        {
            ptr_reserved += sizeof(NodeType);
            throw std::bad_alloc{};
        }

        // create node
        auto *pnode = new (ptr_reserved) NodeType{std::forward<ParamTypes>(params)...}; // forward - hace cast solo a los que son referencias r_value y no a los que son l_value
        // Si a un unique_ptr le das una direccion de memoria se la queda y la gestiona      //devuelve un puntero crudo - unicamente una direccion de memoria
        // crea un unique_ptr, que va a apuntar a un nodo que se inicializa a esta direccion de memoria que obtiene

        // almacenamos nodo en nuestro espacio de nodos
        nodes.emplace_back(pnode);

        // Return nodes
        return *pnode;
    }

    std::string currentnode{};

private:
    // std::size_t     mem_size        {1024};                                        //  El orden de estas lienas afecta para crear y inicializar
    // MemoryStorage_t mem             {std::make_unique<std::byte[]>(mem_size)};     //\\      en el mismo orden en como aparecen
    // std::byte*      ptr_reserved    {mem.get() + mem_size};                      // puntero que apunta la final de la memoria reservada para el arbol
    std::size_t mem_size{2048};
    MemoryStorage_t mem{std::make_unique<std::byte[]>(mem_size)};
    std::byte *ptr_reserved{mem.get() + mem_size};

    NodeStorage_t nodes{};
};