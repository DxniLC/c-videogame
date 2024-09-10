#pragma once

#include <engines/graphic/SGEngine/scene/entity/particle/ParticleEffect.hpp>

struct FireParticle : ParticleEffect
{
    explicit FireParticle() = default;

    void applyEffect(Particle *particle, float const &deltaTime) override final
    {
        particle->position.y += 0.5f * deltaTime;
        particle->size += glm::vec2(0.1f, 0.1f) * deltaTime;

        double lifePercentage = particle->m_Life / particle->InitialLife;
        glm::vec4 startColor(1.0f, 0.5f, 0.0f, 1.0f);
        glm::vec4 endColor(0.5f, 0.5f, 0.5f, 1.0f);
        // No se actualiza el color de normal. Hay que guardarlo en buffer
        particle->color = glm::mix(startColor, endColor, lifePercentage);
    }
};