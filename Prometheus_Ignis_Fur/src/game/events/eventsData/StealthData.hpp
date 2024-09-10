#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct StealthData : EventData
{
    explicit StealthData(bool active)
        : EventData{EventIds::StealthMode}, active{active}
    {
    }

    bool active;
};