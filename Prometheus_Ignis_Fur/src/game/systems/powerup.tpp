#include "powerup.hpp"

#include <game/component/powerup/powerup.hpp>

#include <game/events/eventsData/UpdatePowerUpUIData.hpp>

template <typename GameCTX>
void PowerUpSystem<GameCTX>::update(GameCTX &g)
{

    for (auto &pwrCMP : g.template getComponents<PowerUpComponent>())
    {
        auto *entity = g.getEntityByKey(pwrCMP.getEntityKey());
        int index{0};
        for (auto itr = pwrCMP.powerUps.begin(); itr < pwrCMP.powerUps.end();)
        {
            bool IsEnded = (*itr)->updatePowerUp(*entity);
            if (IsEnded)
            {

                itr = pwrCMP.powerUps.erase(itr);
                // lanzar evento para que en hud sepa que tiene que borrar el elemento

                auto *eventManager = g.template getSingletonComponent<EventManager>();
                if (eventManager)
                    eventManager->EventQueueInstance.EnqueueLateEvent(UpdatePowerUpUIData{index, false, true});
                    
                pwrCMP.IsUsingPowerUp = false;
            }
            else
            {
                itr++;
                index++;
            }
        }
    }
}
