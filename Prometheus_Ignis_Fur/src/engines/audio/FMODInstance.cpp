
#include "FMODInstance.hpp"

#include <iostream>
#include <glm/glm.hpp>

FMODInstance::FMODInstance(FMOD_STUDIO_EVENTINSTANCE *eventInstance) : eventInstance{eventInstance} {}

FMODInstance::~FMODInstance()
{

    clearEffects();
    stop();
    FMOD_Studio_EventInstance_Release(eventInstance);
}

bool FMODInstance::start()
{
    FMOD_STUDIO_PLAYBACK_STATE state;
    FMOD_Studio_EventInstance_GetPlaybackState(eventInstance, &state);
    if (state != FMOD_STUDIO_PLAYBACK_PLAYING && state != FMOD_STUDIO_PLAYBACK_STARTING)
    {
        FMOD_Studio_EventInstance_Start(eventInstance);

        return false;
    }
    // true si ya se ha iniciado
    return true;
}

bool FMODInstance::IsPlaying()
{
    FMOD_STUDIO_PLAYBACK_STATE state;
    FMOD_Studio_EventInstance_GetPlaybackState(eventInstance, &state);
    return (state == FMOD_STUDIO_PLAYBACK_PLAYING || state == FMOD_STUDIO_PLAYBACK_STARTING);
}

void FMODInstance::stop()
{
    FMOD_STUDIO_PLAYBACK_STATE state;
    FMOD_Studio_EventInstance_GetPlaybackState(eventInstance, &state);
    if (state == FMOD_STUDIO_PLAYBACK_PLAYING || state == FMOD_STUDIO_PLAYBACK_STARTING)

        FMOD_Studio_EventInstance_Stop(eventInstance, FMOD_STUDIO_STOP_ALLOWFADEOUT);
}

void FMODInstance::setVolume(float newVolume)
{
    FMOD_Studio_EventInstance_SetVolume(eventInstance, newVolume);
}

void FMODInstance::setParameterbyName(const char *name, float n)
{

    FMOD_Studio_EventInstance_SetParameterByName(eventInstance, name, n, false);
}

void FMODInstance::set3DPosition(glm::vec3 pos, glm::vec3 direction)
{

    FMOD_3D_ATTRIBUTES l_eventAttributes{
        FMOD_VECTOR{-pos.x, pos.y, pos.z},
        FMOD_VECTOR{0.0f, 0.0f, 0.0f},
        FMOD_VECTOR{direction.x, direction.y, direction.z},
        FMOD_VECTOR{0, 1, 0}};

    FMOD_Studio_EventInstance_Set3DAttributes(eventInstance, &l_eventAttributes);
}

void FMODInstance::setLowPass(float cutoff)
{
    HasLowPass = true;

    FMOD_SYSTEM *coreSystem{nullptr};

    auto *group = CanApplyDSP(FMOD_DSP_TYPE_LOWPASS_SIMPLE);
    if (group)
    {
        FMOD_DSP *dsp{nullptr};

        FMOD_ChannelGroup_GetSystemObject(group, &coreSystem);

        FMOD_System_CreateDSPByType(coreSystem, FMOD_DSP_TYPE_LOWPASS_SIMPLE, &dsp);

        FMOD_DSP_SetParameterFloat(dsp, FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, cutoff);

        FMOD_ChannelGroup_AddDSP(group, 0, dsp);
    }
}

void FMODInstance::clearLowPass()
{

    if (not HasLowPass)
        return;

    HasLowPass = false;

    ClearDSP(FMOD_DSP_TYPE_LOWPASS_SIMPLE);
}

void FMODInstance::setReverb(float level)
{
    HasReverb = true;

    FMOD_SYSTEM *coreSystem{nullptr};

    auto *group = CanApplyDSP(FMOD_DSP_TYPE_SFXREVERB);
    if (group)
    {
        FMOD_ChannelGroup_GetSystemObject(group, &coreSystem);

        FMOD_DSP *dsp{nullptr};

        FMOD_System_CreateDSPByType(coreSystem, FMOD_DSP_TYPE_SFXREVERB, &dsp);

        FMOD_DSP_SetParameterFloat(dsp, FMOD_DSP_SFXREVERB_WETLEVEL, level);

        FMOD_ChannelGroup_AddDSP(group, 0, dsp);
    }
}

void FMODInstance::clearReverb()
{

    if (not HasReverb)
        return;

    HasReverb = false;

    ClearDSP(FMOD_DSP_TYPE_SFXREVERB);
}

FMOD_CHANNELGROUP *FMODInstance::CanApplyDSP(FMOD_DSP_TYPE typeToApply)
{
    FMOD_CHANNELGROUP *group{nullptr};

    FMOD_Studio_EventInstance_GetChannelGroup(eventInstance, &group);

    if (group)
    {

        int numDSPs{0};
        FMOD_ChannelGroup_GetNumDSPs(group, &numDSPs);
        for (int i = 0; i < numDSPs; i++)
        {
            FMOD_DSP *dsp{nullptr};

            FMOD_ChannelGroup_GetDSP(group, i, &dsp);

            FMOD_DSP_TYPE type;

            FMOD_DSP_GetType(dsp, &type);

            if (type == typeToApply)
                return nullptr;
        }
    }
    return group;
}

void FMODInstance::ClearDSP(FMOD_DSP_TYPE typeToClear)
{
    FMOD_CHANNELGROUP *group{nullptr};

    FMOD_Studio_EventInstance_GetChannelGroup(eventInstance, &group);

    if (group)
    {
        int numDSPs{0};
        FMOD_ChannelGroup_GetNumDSPs(group, &numDSPs);

        for (int i = 0; i < numDSPs; i++)
        {
            FMOD_DSP *dsp{nullptr};
            FMOD_ChannelGroup_GetDSP(group, i, &dsp);

            FMOD_DSP_TYPE type;
            FMOD_DSP_GetType(dsp, &type);

            if (type == typeToClear)
            {
                FMOD_ChannelGroup_RemoveDSP(group, dsp);
                FMOD_DSP_Release(dsp);
                break;
            }
        }
    }
}

void FMODInstance::clearEffects()
{
    clearLowPass();
    clearReverb();
}
