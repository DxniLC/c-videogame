
#include "event.hpp"

void EventSystem::updateTriggers(ECS::EntityManager &entityMan)
{
    for (auto &event_ptr : *eventManager)
    {
        (*event_ptr).handle(entityMan);
    }
    eventManager->clearEventTriggers();
}

void EventSystem::update()
{
    eventManager->EventQueueInstance.update();
}

void EventSystem::lateUpdate()
{
    eventManager->EventQueueInstance.lateUpdate();
}
