#pragma once

#include <glm/glm.hpp>

#include <random>

struct Particle
{
    explicit Particle(glm::vec3 position, glm::vec2 size, double m_life);

    bool update(float const &deltaTime);

    glm::vec4 color;

    glm::vec3 position;

    glm::vec2 size;

    double m_Life;

    double InitialLife;

    glm::vec3 randMovement;

    bool randomized{false};

};