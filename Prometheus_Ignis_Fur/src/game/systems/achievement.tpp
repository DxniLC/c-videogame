#include "achievement.hpp"

#include <game/events/eventsData/AchievementEventData.hpp>

template <typename GameCTX>
void AchievementSystem<GameCTX>::update(GameCTX &g)
{
    AchievementManager *achievementManager = g.template getSingletonComponent<AchievementManager>();

    if (achievementManager)
    {
        for (auto itr = achievementManager->begin(); itr != achievementManager->end(); itr++)
        {
            if(itr->second->IsComplete())
                continue;

            bool Completed = itr->second->update(achievementManager->achievementData);

            if (Completed)
            {
                auto *eventManager = g.template getSingletonComponent<EventManager>();
                if (eventManager)
                {
                    eventManager->EventQueueInstance.EnqueueLateEvent(AchievementEventData{itr->second->AchievementID, itr->second->iconPath});
                }
            }
        }
    }
}