#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct InitCinematicData : EventData
{
    explicit InitCinematicData(unsigned int CinematicID, float MaxDuration)
        : EventData{EventIds::InitCinematic}, CinematicID{CinematicID}, MaxDuration{MaxDuration}
    {
    }

    unsigned int CinematicID;
    float MaxDuration;
};