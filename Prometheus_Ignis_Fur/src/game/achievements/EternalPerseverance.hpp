#pragma once

#include "Achievement.hpp"
#include <cstdint>

// Jump 30 times
struct EternalPerseverance : Achievement
{
    explicit EternalPerseverance()
    {
        AchievementID = "Perseverance";
        iconPath = "media/textures/Achievements/EternalPerseverance.png";
    };

    bool update(AchievementData &achievementData) override final
    {


        currentDeath = achievementData.currentDeaths;
        if (achievementData.currentDeaths >= maxDeaths)
            IsCompleted = true;

        return IsCompleted;
    }

    void SaveAchievement(Json::Value &element) override final
    {
        if (IsCompleted)
        {
            element["CurrentDeaths"] = maxDeaths;
        }
        else
        {
            element["CurrentDeaths"] = currentDeath;
        }
    }

private:
    uint8_t maxDeaths{5};
    int currentDeath{0};
};