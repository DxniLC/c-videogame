#pragma once

#include <string>
#include <json/value.h>
#include <vector>

struct SaveData
{

    Json::Value Saving()
    {
        Json::Value data{};
        data["Level"] = level;
        data["KeyData"] = keyData;
        data["Abilities"] = abilities;
        for (auto &idPower : powers)
            data["PowerUps"].append(idPower);
        return data;
    }

    std::string level{};
    std::string keyData{};
    int abilities{};
    std::vector<PowerUpId> powers{};
};