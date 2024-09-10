#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct ShowBoardData : EventData
{
    explicit ShowBoardData(const char *path, glm::vec3 position, glm::vec2 size, glm::vec3 locked_axis, EntityKey EntityAsociated)
        : EventData{EventIds::ShowControlBoard}, path{path}, position{position}, size{size}, locked_axis{locked_axis}, EntityAsociated{EntityAsociated}
    {
    }

    std::string path;
    glm::vec3 position;
    glm::vec2 size;
    glm::vec3 locked_axis;
    EntityKey EntityAsociated;
};