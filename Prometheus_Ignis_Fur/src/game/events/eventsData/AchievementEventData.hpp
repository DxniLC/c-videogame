#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct AchievementEventData : EventData
{
    explicit AchievementEventData(std::string achievementID, std::string iconPath)
        : EventData{EventIds::ShowAchievement}, achievementID{achievementID}, iconPath{iconPath}
    {
    }
    std::string achievementID;
    std::string iconPath;
};