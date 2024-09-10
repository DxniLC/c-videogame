#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <glm/vec3.hpp>

struct ProjectileEventData : EventData
{
    explicit ProjectileEventData(glm::vec3 position, glm::vec3 direction, float inputForce, bool hasGravity, bool hasFriction)
        : EventData{EventIds::CreateProjectile}, position{position}, direction{direction}, inputForce{inputForce}, hasGravity{hasGravity}, hasFriction{hasFriction}
    {
    }

    glm::vec3 position;
    glm::vec3 direction;
    float inputForce;
    bool hasGravity;
    bool hasFriction;
};