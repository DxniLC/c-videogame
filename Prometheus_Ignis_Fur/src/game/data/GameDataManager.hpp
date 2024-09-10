#pragma once

#include "SaveData.hpp"

#include <util/JSONHelper.hpp>

#include "WebManager.hpp"

#include <fstream>
#include <ctime>


struct GameDataManager
{

    explicit GameDataManager(std::string gamePath = "media/game/SaveData.json", std::string achievementPath = "media/game/Achievements.json")
        : pathData{gamePath}, pathAchievement{achievementPath} {}

    void SaveGame(SaveData saveData)
    {
        saveData.keyData = GetKeyData();

        std::ofstream outputFile(pathData);
        outputFile << saveData.Saving();
        outputFile.close();
    }

    bool HasASaveGame()
    {

        std::ifstream file(pathData);
        if (file)
        {
            file.close();
            return true;
        }

        return false;
    }

    Json::Value LoadGame()
    {

        std::ifstream file(pathData);
        if (file)
        {
            Json::Reader reader;
            reader.parse(file, gameData);
        }
        else
        {
            // Empeiza desde Tutorial
            keyData = GenerateKey();

            gameData["KeyData"] = keyData;
            gameData["Level"] = "Tutorial";
            gameData["Abilities"] = 0;
            gameData["PowerUps"] = Json::arrayValue;

            std::ofstream outputFile(pathData);
            outputFile << gameData;
            outputFile.close();
        }
        file.close();
        return gameData;
    }
    void UploadOnWeb(Json::Value const &achievementData)
    {
        if (webManager.Connect("localhost:3306", "root", "", "webscythe"))
        {
            if (keyData.empty())
                keyData = GetKeyData();
            std::cout << keyData << "\n";
            webManager.UpdateProgress(keyData, achievementData);
            webManager.Disconnect();
        }
    }

    [[nodiscard]] std::pair<std::string, std::string> getUserInfo()
    {
        LoadGame();
        std::string userName{"Not Connected"};
        if (webManager.Connect("localhost:3306", "root", "", "webscythe"))
        {
            userName = webManager.getUsername(keyData);
            webManager.Disconnect();
        }
        return std::make_pair(userName, keyData);
    }

    [[nodiscard]] constexpr std::string getAchievementPath() const noexcept
    {
        return pathAchievement;
    }

    [[nodiscard]] constexpr std::string getGameDataPath() const noexcept
    {
        return pathData;
    }

private:
    std::string const pathData{"media/game/SaveData.json"};
    std::string const pathAchievement{"media/game/Achievements.json"};

    std::string keyData{};
    Json::Value gameData{};
    WebManager webManager{};

    std::string GetKeyData()
    {
        std::ifstream file(pathData);
        if (file)
        {
            Json::Value jsonData;
            Json::Reader reader;
            reader.parse(file, jsonData);
            file.close();

            keyData = jsonData["KeyData"].asString();
        }
        else
        {
            keyData = GenerateKey();

            Json::Value jsonData{};
            jsonData["KeyData"] = keyData;

            std::ofstream outputFile(pathData);
            outputFile << jsonData;
            outputFile.close();
        }
        return keyData;
    }

    std::string GenerateKey()
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        int numDigits{12};

        long int randomKey{0};
        for (int i = 0; i < numDigits; ++i)
        {
            randomKey = (randomKey * 10) + (std::rand() % 10);
        }

        return std::to_string(std::abs(randomKey));
    }
};
