#pragma once

#include <ecs/util/typealiases.hpp>
#include <array>
#include <unordered_map>

#include <engines/graphic/SGEngine/SGEngine.hpp>

#include <game/manager/event/EventManager.hpp>

#include <game/component/powerup/PowerUpId.hpp>
#include <game/component/ability/abilities/AbilityID.hpp>

namespace ECS
{
    struct EntityManager;
};

struct HUDSystem : EventObserver
{

    explicit HUDSystem(HUDEngine *hudEngine, SGEngine *engine, EventManager &eventManager);

    void Process(EventData *eventData) override final;

    void draw(ECS::EntityManager &entityManager);

private:
    EntityKey e_key;

    int PowerUpIndex{0};

    SGEngine *eng{nullptr};

    std::unordered_map<PowerUpId, ResourceTexture *> powerUpsImages{};

    std::unordered_map<AbilityID, HUDImage *> ablityElements{};
    std::unordered_map<AbilityID, std::pair<ResourceTexture *, ResourceTexture *>> ablityImages{};

    ResourceTexture *powerUpUsing{nullptr};
    ResourceTexture *powerUpSelected{nullptr};

    HUDImage *baseHud{nullptr};

    std::array<HUDImage *, 3> powerUpsHUD;
    std::array<HUDImage *, 3> slot;
};