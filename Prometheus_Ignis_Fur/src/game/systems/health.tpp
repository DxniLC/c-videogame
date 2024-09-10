#include "health.hpp"

#include <game/component/health.hpp>

#include <game/events/GoToCheckpoint.hpp>

template <typename GameCTX>
HealthSystem<GameCTX>::HealthSystem(EventManager &eventManager)
{
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);
}

template <typename GameCTX>
void HealthSystem<GameCTX>::update(GameCTX &g)
{
    if (IsLocked)
        return;

    for (auto &health : g.template getComponents<HealthComponent>())
    {

        if (health.damaged)
        {
            if (health.initInmunity)
            {
                health.mStartTime = std::chrono::system_clock::now();
                health.initInmunity = false;

                health.HP -= health.damage;

                auto *entity = g.getEntityByKey(health.getEntityKey());
                if (entity->type == EntityType::PLAYER)
                {
                    auto *audio = entity->template getComponent<AudioComponent>();
                    if (audio)
                        audio->soundsToUpdate.emplace_back("Voices/player_damage", true, nullptr);
                }

                // auto *entity = g.getEntityByKey(health.getEntityKey());
                if (health.HP <= 0)
                {
                    // Delete Entity with animation
                    // EventManager::addTriggerEvent(GoToCheckpointEvent{*entity});
                    // Ya lo anade colision y por tanto ya volveria al spawn. almenos si cae a VOID
                }
            }
            else
            {
                health.mDeltaTime = std::chrono::system_clock::now() - health.mStartTime;

                if (health.mDeltaTime.count() >= health.inmunityTime)
                {
                    health.damaged = false;
                }
            }
        }
    }
}

template <typename GameCTX>
void HealthSystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
            IsLocked = data->status;
        else
            std::cout << "\n***** ERROR LockSystem Health *****\n";
    }
}
