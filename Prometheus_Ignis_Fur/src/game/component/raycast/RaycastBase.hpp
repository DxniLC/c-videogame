
#pragma once

#include <game/manager/rules/FunctionIDs.hpp>
#include "RaycastResult.hpp"

using RayFlagsType = uint8_t;

enum RayCollideFlags : RayFlagsType
{
    RAY_SCENE = 1 << 0,
    RAY_WORLD = 1 << 1,
    RAY_BODY = 1 << 2,

    RAY_ALL = (1 << 4) - 1
};

struct Raycast
{
    explicit Raycast(glm::vec3 origin, glm::vec3 direction, float range) : origin(origin), direction(direction), range(range) {}
    explicit Raycast(glm::vec3 origin, glm::vec3 direction, float range, RayFlagsType flags) : origin(origin), direction(direction), range(range), rayFlags(flags) {}
    explicit Raycast(glm::vec3 origin, glm::vec3 direction, float range, FunctionID functionID, RayFlagsType flags = RAY_ALL) : origin(origin), direction(direction), range(range), functionID(functionID), rayFlags(flags) {}

    // Origin raycast on Local
    glm::vec3 origin{};
    glm::vec3 direction{};
    float range{3};

    // Add a unique function to execute when ray collide
    FunctionID functionID{FunctionID::NONEFUNC};

    // Flags to know the ray has to collide with
    RayFlagsType rayFlags{RayCollideFlags::RAY_ALL};

    // Result of Raycast
    RaycastResult result{};

    bool IsActive{true};
};
