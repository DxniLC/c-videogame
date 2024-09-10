#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct LoadLevelData : EventData
{
    explicit LoadLevelData(std::string levelName)
        : EventData{EventIds::LoadLevel}, levelName{levelName}
    {
    }

    std::string levelName;
};