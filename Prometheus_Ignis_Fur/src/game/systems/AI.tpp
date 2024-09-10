
#include "AI.hpp"
#include <game/component/sensory.hpp>
#include <game/component/AI.hpp>

// EVENT DATAS
#include <game/events/eventsData/BBTargetData.hpp>

template <typename GameCTX>
AISystem<GameCTX>::AISystem(EventManager &eventMan)
{
    eventMan.EventQueueInstance.Subscribe(EventIds::BBTarget, this);
}

template <typename GameCTX>
void AISystem<GameCTX>::update(GameCTX &g, float const &deltaTime)
{

    // Update Blackboard
    auto *blackboard = g.template getSingletonComponent<Blackboard>();

    // Update BehaviourTrees
    for (auto &aiCMP : g.template getComponents<AIComponent>())
    {
        auto *entity = g.template getEntityByKey(aiCMP.getEntityKey());
        if (entity)
            aiCMP.BehaviourTree->run(*entity, g);
    }

    // ResetBlackBoard
    if (blackboard)
    {
        blackboard->someCalculatedPath = false;

        if (blackboard->targetAction)
        {

            blackboard->targetAction = false;
        }
    }
}

template <typename GameCTX>
void AISystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::BBTarget)
    {
        auto *data = dynamic_cast<BBTargetData *>(eventData);
        if (data)
        {
            data->blackboard->targetAction = true;
        }
    }
}
