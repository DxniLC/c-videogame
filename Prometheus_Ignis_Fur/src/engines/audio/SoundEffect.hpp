
#pragma once

#include "AudioInstance.hpp"

struct SoundEffect
{
    virtual ~SoundEffect() = default;
    virtual void update(AudioInstance *audioInstance) = 0;
    virtual void clearEffect(AudioInstance *audioInstance) = 0;
};