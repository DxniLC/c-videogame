#pragma once
#include <cstdint>
#include <string>

#include <ecs/manager/entitymanager.hpp>
#include <game/manager/event/EventManager.hpp>

enum class BTNodeStatus_t : uint8_t
{ // como solo vamos a almacenar 1, 2 y 3 no necesitamos lso 32 bits generales de u nint, con 8 svale
    // el estad oqeu devuelve a arriba -> en BT éxito, ejecutando o fracaso
        success
    ,   fail
    ,   running
};

struct BTNode_t
{
    struct Deleter
    { void operator()(BTNode_t *n) { n->~BTNode_t(); } };

    BTNode_t()          = default;
    virtual ~BTNode_t() = default;

    BTNode_t(const BTNode_t&)               = delete;
    BTNode_t(BTNode_t &&)                   = delete;
    BTNode_t& operator=(const BTNode_t&)    = delete;
    BTNode_t& operator=(BTNode_t&&)         = delete;

    std::string currentNodeName;

    virtual BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept = 0;
    BTNodeStatus_t run(ECS::Entity &&e, ECS::EntityManager &&entityManager) noexcept
    {
        return run(e, entityManager);

    };
};