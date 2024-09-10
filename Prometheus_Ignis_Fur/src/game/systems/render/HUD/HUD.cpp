
#include "HUD.hpp"

#include <engines/graphic/SGEngine/HUD/HUDEngine.hpp>

#include <ecs/manager/entitymanager.hpp>

#include <game/component/health.hpp>

#include <game/component/ability/ability.hpp>
#include <game/component/powerup/powerup.hpp>

#include <game/events/eventsData/PowerUpData.hpp>
#include <game/events/eventsData/UpdatePowerUpUIData.hpp>
#include <game/events/eventsData/LockSystemData.hpp>
#include <game/events/eventsData/AbilityData.hpp>

HUDSystem::HUDSystem(HUDEngine *hudEngine, SGEngine *engine, EventManager &eventManager)
{
    this->eng = engine;

    powerUpsHUD[0] = hudEngine->addImage(nullptr, {0.713, 0.85}, {0.0465, 0.06}, 0);
    powerUpsHUD[1] = hudEngine->addImage(nullptr, {0.76, 0.85}, {0.0465, 0.06}, 0);
    powerUpsHUD[2] = hudEngine->addImage(nullptr, {0.807, 0.85}, {0.0465, 0.06}, 0);

    powerUpSelected = engine->getTexture("media/textures/HUD/PowerUpSelected.png");
    powerUpUsing = engine->getTexture("media/textures/HUD/PowerUpUsing.png");

    baseHud = hudEngine->addImage("media/textures/HUD/HUD.png", {0.83, 0.85}, {0.3, 0.2}, 1);

    slot[0] = hudEngine->addImage(powerUpSelected, {0.7135, 0.85}, {0.0482, 0.065}, 2);
    slot[1] = hudEngine->addImage(nullptr, {0.7605, 0.85}, {0.0482, 0.065}, 2);
    slot[2] = hudEngine->addImage(nullptr, {0.8075, 0.85}, {0.0482, 0.065}, 2);

    powerUpsImages = {
        {PowerUpId::MOREJUMP, engine->getTexture("media/textures/HUD/PowerUpsSalto.png")},
        {PowerUpId::MORESPEED, engine->getTexture("media/textures/HUD/PowerUpVelocity.png")},
    };

    ablityElements = {
        {AbilityID::DASH_ABILITY, hudEngine->addImage(nullptr, {0.865, 0.8}, {0.072, 0.09}, 2)},
        {AbilityID::DOUBLE_JUMP_ABILITY, hudEngine->addImage(nullptr, {0.941, 0.8}, {0.072, 0.09}, 2)},

    };

    ablityImages = {
        {AbilityID::DASH_ABILITY, std::make_pair(engine->getTexture("media/textures/Divine/HUDDash_Trans.png"), engine->getTexture("media/textures/Divine/HUDDash.png"))},
        {AbilityID::DOUBLE_JUMP_ABILITY, std::make_pair(engine->getTexture("media/textures/Divine/HUDDobleSalto_Trans.png"), engine->getTexture("media/textures/Divine/HUDDobleSalto.png"))},
    };

    eventManager.EventQueueInstance.Subscribe(EventIds::ElementChest, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::UpdatePowerUpUI, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::UpdateAbility, this);
}

void HUDSystem::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::ElementChest)
    {
        auto *data = dynamic_cast<PowerUpData *>(eventData);
        if (data)
        {

            for (auto &element : powerUpsHUD)
            {
                if (not element->image)
                {
                    element->image = powerUpsImages[data->powerUpType];
                    break;
                }
            }
        }
    }
    else if (eventData->EventId == EventIds::UpdatePowerUpUI)
    {
        auto *data = dynamic_cast<UpdatePowerUpUIData *>(eventData);
        if (data)
        {

            if (data->remove)
            {
                powerUpsHUD[data->index]->image = nullptr;
                slot[data->index]->image = powerUpSelected;
            }
            else if (data->use)
            {
                slot[data->index]->image = powerUpUsing;
            }
            else
            {
                std::size_t index = (data->index - 1 < 0) ? slot.size() - 1 : data->index - 1;
                slot[index]->image = nullptr;
                slot[data->index]->image = powerUpSelected;
            }
        }
    }
    else if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
        {
            for (auto &powerUp : powerUpsHUD)
                powerUp->IsActive = not data->status;

            for (auto &element : slot)
                element->IsActive = not data->status;

            for (auto &element : ablityElements)
                element.second->IsActive = not data->status;

            baseHud->IsActive = not data->status;
        }
        else
            std::cout << "\n***** ERROR LockSystem Health *****\n";
    }
    else if (eventData->EventId == EventIds::UpdateAbility)
    {
        AbilityData *data{dynamic_cast<AbilityData *>(eventData)};
        if (data)
        {
            if (data->used)
            {
                ablityElements[data->abilityID]->image = ablityImages[data->abilityID].first;
            }
            else
            {
                ablityElements[data->abilityID]->image = ablityImages[data->abilityID].second;
            }
        }
    }
}

void HUDSystem::draw(ECS::EntityManager &entityManager)
{
}