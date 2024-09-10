#pragma once

#include <engines/graphic/SGEngine/scene/entity/particle/ParticleEffect.hpp>

struct DirtParticle : ParticleEffect
{
    explicit DirtParticle() = default;

    void applyEffect(Particle *particle, float const &deltaTime) override final
    {

        if (!particle->randomized)
        {
            float maxChange{0.5f};
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-maxChange, maxChange);
            float changex = dis(gen);
            float changez = dis(gen);
            particle->position.x += changex;
            particle->position.z += changez;

            float maxAngle{15};
            std::uniform_real_distribution<float> dis2(-maxAngle, maxAngle);
            float inclination = dis2(gen);
            float desviation = glm::radians(dis(gen)); // Genera un ángulo aleatorio en radianes

            float x = std::sin(desviation) * std::cos(inclination);
            float y = std::sin(inclination);
            float z = std::cos(desviation) * std::cos(inclination);
            particle->randMovement = glm::normalize(glm::vec3(x, y, z));
            particle->randomized = true;
        }

        particle->position += particle->randMovement * 3.f * deltaTime;
        particle->size -= glm::vec2(0.1f, 0.1f) * deltaTime;

        double lifePercentage = particle->m_Life / particle->InitialLife;
        glm::vec4 startColor(1.0f, 0.5f, 0.0f, 1.0f);
        glm::vec4 endColor(0.5f, 0.5f, 0.5f, 1.0f);
        // No se actualiza el color de normal. Hay que guardarlo en buffer
        particle->color = glm::mix(startColor, endColor, lifePercentage);
    }
};