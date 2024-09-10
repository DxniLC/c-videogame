#pragma once

#include <engines/audio/SoundEffect.hpp>

#include <engines/audio/FMODInstance.hpp>

struct LowPassEffect : SoundEffect
{
    LowPassEffect(float d) : cuttof(d) {}

    void update(AudioInstance *audioInstance) override
    {
        audioInstance->setLowPass(cuttof);
    }

    void clearEffect(AudioInstance *audioInstance) override
    {
        audioInstance->clearLowPass();
    }

    float cuttof{750.0f};
};