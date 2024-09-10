#pragma once

#include <game/data/AchievementData.hpp>
#include <json/value.h>

struct Achievement
{
    virtual ~Achievement() = default;

    // Return true if has finished
    virtual bool update(AchievementData &achievementData) = 0;

    void SaveAchievementOnData(Json::Value &element)
    {
        element["Obtained"] = IsCompleted;
        if (IsCompleted)
            element["Date"] = GetCurrentDate();

        auto totalTime = element["Time"].asInt() + timer.ellapsedDuration<ChronoTime::Seconds>().count();
        element["Time"] = totalTime;

        SaveAchievement(element);
    }

    constexpr bool IsComplete() const noexcept { return IsCompleted; };

    std::string AchievementID{};
    
    std::string iconPath{};

    GameTimer timer{};

protected:
    bool IsCompleted{false};

    virtual void SaveAchievement(Json::Value &element) = 0;

    std::string GetCurrentDate()
    {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);

        return {buffer};
    }
};