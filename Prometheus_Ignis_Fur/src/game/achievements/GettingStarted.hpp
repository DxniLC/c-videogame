#pragma once

#include "Achievement.hpp"
#include <cstdint>

// Jump 30 times
struct GettingStarted : Achievement
{
    explicit GettingStarted()
    {
        AchievementID = "GettingStarted";
        iconPath = "media/textures/Achievements/GettingStarted.png";
    };

    bool update(AchievementData &achievementData) override final
    {
        levelname = achievementData.lastlevel;


        if (levelname == "Tutorial")
        {

            IsCompleted = true;
        }
            

        return IsCompleted;
    }

    void SaveAchievement(Json::Value &element) override final
    {}

private:

    std::string levelname{};
};