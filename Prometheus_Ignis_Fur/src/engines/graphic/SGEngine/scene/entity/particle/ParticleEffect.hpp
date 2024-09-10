#pragma once

#include "Particle.hpp"

struct ParticleEffect
{
    virtual ~ParticleEffect() = default;
    virtual void applyEffect(Particle *particle, float const &deltaTime) = 0;
};