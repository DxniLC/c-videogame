#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct CaveData : EventData
{
    explicit CaveData(bool status, float level)
        : EventData{EventIds::OnCave}, status{status}, level{level}
    {
    }

    bool status;
    float level;
};