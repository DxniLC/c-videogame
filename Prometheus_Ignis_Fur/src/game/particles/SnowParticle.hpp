#pragma once

#include <engines/graphic/SGEngine/scene/entity/particle/ParticleEffect.hpp>
#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>


struct SnowParticle : ParticleEffect
{
    explicit SnowParticle(TNode *node) : nodeP{node}{};

    void applyEffect(Particle *particle, float const &deltaTime) override final
    {

        if (!particle->randomized)
        {
            auto *camera = nodeP->getEntity<TCamera>();
            glm::vec3 front{nodeP->direction};

            if (camera)
            {
                front=camera->Front;
            }
            float maxChange{12.f};
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-maxChange, maxChange);
            float changex = dis(gen);
            float changey = dis(gen);
            float changez = dis(gen);
            particle->position.x += changex;
            particle->position.y += (changey / 2)+4;
            particle->position.z += changez;
            particle->position += glm::normalize(front) * maxChange ;

            float sizeChanger{0.2f};
            std::uniform_real_distribution<float> dis2(0, sizeChanger);
            float change = dis2(gen);
            particle->size += change;

            particle->randomized = true;
        }

        particle->position.y -= 10.f * deltaTime;

        double lifePercentage = particle->m_Life / particle->InitialLife;

        glm::vec4 startColor(1.0f, 0.5f, 0.0f, 1.0f);
        glm::vec4 endColor(0.5f, 0.5f, 0.5f, 1.0f);
        // No se actualiza el color de normal. Hay que guardarlo en buffer
        particle->color = glm::mix(startColor, endColor, lifePercentage);
    }
    TNode *nodeP{nullptr};
};