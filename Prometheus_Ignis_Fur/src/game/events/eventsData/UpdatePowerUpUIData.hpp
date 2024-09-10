
#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct UpdatePowerUpUIData : EventData
{
    explicit UpdatePowerUpUIData(int index, bool use = false, bool remove = false)
        : EventData{EventIds::UpdatePowerUpUI}, index{index}, use{use}, remove{remove}
    {
    }
    int index;
    bool use;
    bool remove;
};
