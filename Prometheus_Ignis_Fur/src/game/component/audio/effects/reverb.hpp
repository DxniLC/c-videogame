#pragma once

#include <engines/audio/SoundEffect.hpp>

#include <engines/audio/FMODInstance.hpp>

struct ReverbEffect : SoundEffect
{
    ReverbEffect(float d) : wetLevel(d) {}

    void update(AudioInstance *audioInstance) override
    {
        audioInstance->setReverb(wetLevel);
    }

    void clearEffect(AudioInstance *audioInstance) override
    {
        audioInstance->clearReverb();
    }

    float wetLevel{0.5f};
};