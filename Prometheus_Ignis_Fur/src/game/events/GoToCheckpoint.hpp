#pragma once

#include <game/manager/event/EventTrigger.hpp>

#include <game/component/checkpoint.hpp>
#include <game/component/render.hpp>

struct GoToCheckpointEvent : public EventTrigger
{

    // TODO: Hacer un bool para saber si checkpoint o spawn
    explicit GoToCheckpointEvent(ECS::Entity &A) : ent_A(A) {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *checkpoint = ent_A.getComponent<CheckPointComponent>();

        if (checkpoint)
        {
            auto *render = ent_A.getComponent<RenderComponent>();
            auto *health = ent_A.getComponent<HealthComponent>();
            if (health)
            {
                if (health->HP <= 0)
                {
                    render->node->setGlobalPosition(checkpoint->spawnLevel.first);
                    health->HP = checkpoint->spawnLevel.second;
                }
                else
                {
                    render->node->setGlobalPosition(checkpoint->checkpointSpawn.first);
                }
            }
            else if (render)
            {
                render->node->setGlobalPosition(checkpoint->spawnLevel.first);
            }
        }
    }
    ECS::Entity ent_A;
};