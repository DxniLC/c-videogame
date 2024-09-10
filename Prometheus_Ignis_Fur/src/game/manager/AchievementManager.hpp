#pragma once

#include <ecs/component/component.hpp>

#include <util/JSONHelper.hpp>

#include <game/achievements/Achievement.hpp>
#include <game/data/AchievementData.hpp>

// Achievement List
#include <game/achievements/Grasshopper.hpp>
#include <game/achievements/GettingStarted.hpp>
#include <game/achievements/EternalPerseverance.hpp>

#include <fstream>

#include <string>
#include <unordered_map>
#include <map>

struct AchievementManager : ECS::SingletonComponentBase<AchievementManager>
{
    explicit AchievementManager() = default;

    void readAchievementFile(std::string pathToRead)
    {
        if (not Initialized)
        {
            achievementPath = pathToRead;
            std::ifstream file(pathToRead);
            if (!file)
                throw std::runtime_error("No se puede abrir el fichero JSON Achievements\n");

            Json::Reader reader;
            reader.parse(file, achievementDataJson);
            file.close();

            Initialized = true;

            for (auto const &element : achievementDataJson)
            {
                if (not element["Obtained"].asBool())
                {
                    std::string ID = element["AchievementID"].asString();
                    auto itr = achievementStorage.find(ID);
                    if (itr != achievementStorage.end())
                    {
                        m_achievements.emplace(ID, itr->second(element, achievementData));
                        // currentAchievement.second->AchievementID = ID;
                    }
                    else
                        throw "\n***** ERROR ACHIEVEMENT NOT FOUND *****\n";
                }
            }
        }
    }

    Json::Value const &SaveAchievementsOnFile()
    {
        if (Initialized)
        {

            for (auto &element : achievementDataJson)
            {
                auto itr = m_achievements.find(element["AchievementID"].asString());
                if (itr != m_achievements.end())
                    itr->second->SaveAchievementOnData(element);
            }

            std::ofstream outputFile(achievementPath);
            outputFile << achievementDataJson;
            outputFile.close();
            Initialized = false;
        }
        return achievementDataJson;
    }

    [[nodiscard]] auto begin() noexcept { return m_achievements.begin(); }
    [[nodiscard]] auto end() noexcept { return m_achievements.end(); }
    [[nodiscard]] auto cbegin() const noexcept { return m_achievements.cbegin(); }
    [[nodiscard]] auto cend() const noexcept { return m_achievements.cend(); }

    Achievement *getAchievement(std::string achievementID)
    {
        auto itr = m_achievements.find(achievementID);
        if (itr != m_achievements.end())
        {
            return itr->second.get();
        }
        return nullptr;
    }

    AchievementData achievementData{};

private:
    std::string achievementPath{};

    bool Initialized{false};

    std::unordered_map<std::string, std::function<std::unique_ptr<Achievement>(Json::Value const &, AchievementData &)>> achievementStorage = {
        {"GrassHopper", [](Json::Value const &element, AchievementData &achieveCMP)
         {
             achieveCMP.currentJumps = element["CurrentJumps"].asInt();
             return std::make_unique<Grasshopper>();
         }},
        {"GettingStarted", [](Json::Value const &element, AchievementData &achieveCMP)
        {
             achieveCMP.lastlevel = element["LastLevel"].asString();
             return std::make_unique<GettingStarted>();
        }},
        {"Perseverance", [](Json::Value const &element, AchievementData &achieveCMP)
        {
             achieveCMP.lastlevel = element["CurrentDeaths"].asString();
             return std::make_unique<EternalPerseverance>();
        }}

    };

    std::map<std::string, std::unique_ptr<Achievement>> m_achievements{};

    Json::Value achievementDataJson;
};
