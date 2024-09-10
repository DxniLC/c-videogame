#pragma once

namespace ECS{
    struct EntityManager;
}
#include <game/manager/event/EventManager.hpp>

struct EventSystem
{
    explicit EventSystem(EventManager &event) : eventManager(&event){}
    void updateTriggers(ECS::EntityManager &entityMan);

    void update();
    void lateUpdate();

private:
    EventManager *eventManager{nullptr};
    
};
