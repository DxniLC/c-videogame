#pragma once

#include <json/json.h>
#include <glm/glm.hpp>

struct JSONHelper
{
    inline static glm::vec3 getVector3(const Json::Value &element)
    {
        return glm::vec3{element[0].asFloat(), element[1].asFloat(), element[2].asFloat()};
    }

    inline static glm::vec2 getVector2(const Json::Value &element)
    {
        return glm::vec2{element[0].asFloat(), element[1].asFloat()};
    }
};
