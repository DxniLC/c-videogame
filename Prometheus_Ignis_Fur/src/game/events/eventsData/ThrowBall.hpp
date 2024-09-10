#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <game/AI/Blackboard.hpp>

struct ThrowBallData : EventData
{
    explicit ThrowBallData(glm::vec3 position, glm::vec3 direction, float force, float potencia)
        : EventData{EventIds::ThrowBall}, position{position}, direction{direction}, force{force},potencia{potencia}
    {
    }
    glm::vec3 position;
    glm::vec3 direction; 
    float force;
    float potencia;
};