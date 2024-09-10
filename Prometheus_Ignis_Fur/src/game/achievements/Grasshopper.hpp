#pragma once

#include "Achievement.hpp"
#include <cstdint>

// Jump 30 times
struct Grasshopper : Achievement
{
    explicit Grasshopper()
    {
        AchievementID = "GrassHopper";
        iconPath = "media/textures/Achievements/GrassHopper.png";
    };

    bool update(AchievementData &achievementData) override final
    {

        currentJump = achievementData.currentJumps;
        if (achievementData.currentJumps >= maxJumps)
            IsCompleted = true;

        return IsCompleted;
    }

    void SaveAchievement(Json::Value &element) override final
    {
        if (IsCompleted)
        {
            element["CurrentJumps"] = maxJumps;
        }
        else
        {
            element["CurrentJumps"] = currentJump;
        }
    }

private:
    uint8_t maxJumps{30};
    int currentJump{0};
};