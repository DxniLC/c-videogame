#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <game/component/ability/abilities/AbilityID.hpp>

struct AbilityData : EventData
{
    explicit AbilityData(AbilityID abilityID, bool used = false)
        : EventData{EventIds::UpdateAbility}, abilityID{abilityID}, used{used}
    {
    }

    AbilityID abilityID;
    bool used;
};