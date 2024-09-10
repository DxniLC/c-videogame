#pragma once

#include <glm/vec3.hpp>

struct AudioInstance
{
    virtual ~AudioInstance() = default;

    virtual bool start() = 0;
    virtual bool IsPlaying() = 0;
    virtual void stop() = 0;

    virtual void setVolume(float newVolume) = 0;

    virtual void set3DPosition(glm::vec3 pos, glm::vec3 direction) = 0;
    
    virtual void setParameterbyName(const char *name, float n) = 0;

    virtual void setLowPass(float cutoff) = 0;
    virtual void clearLowPass() = 0;

    virtual void setReverb(float cutoff) = 0;
    virtual void clearReverb() = 0;

};
