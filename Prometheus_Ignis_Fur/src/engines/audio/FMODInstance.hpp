#pragma once

#include "AudioInstance.hpp"

#include <FMOD/src/fmod.h>
#include <FMOD/src/fmod_studio.h>



struct FMODInstance : AudioInstance
{

    explicit FMODInstance(FMOD_STUDIO_EVENTINSTANCE *eventInstance);

    ~FMODInstance();

    bool start() override final;
    bool IsPlaying() override final;
    void stop() override final;

    void setVolume(float newVolume) override final;


    void setLowPass(float cutoff) override final;
    void clearLowPass() override final;

    void setReverb(float level) override final;
    void clearReverb() override final;
    
    void set3DPosition(glm::vec3 pos, glm::vec3 direction) override final;

    void setParameterbyName(const char *name, float n) override final;

    void clearEffects();

private:

    FMOD_STUDIO_EVENTINSTANCE *eventInstance{nullptr};

    FMOD_CHANNELGROUP * CanApplyDSP(FMOD_DSP_TYPE typeToApply);

    void ClearDSP(FMOD_DSP_TYPE typeToClear);

    bool Has3DInstance{false};
    bool HasLowPass{false};
    bool HasReverb{false};

};