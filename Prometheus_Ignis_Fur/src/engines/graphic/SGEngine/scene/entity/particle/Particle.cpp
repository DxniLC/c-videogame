
#include "Particle.hpp"

#include <random>

#include <GLAD/src/glad.h>

Particle::Particle(glm::vec3 position, glm::vec2 size, double time_life)
    : position{position}, size{size}, m_Life{time_life}, InitialLife{time_life} {}

bool Particle::update(float const &deltaTime)
{
    m_Life -= deltaTime;

    if (m_Life > 0)
        return true;

    return false;
}
