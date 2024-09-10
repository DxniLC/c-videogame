#pragma once

#include <FMOD/src/fmod.h>
#include <FMOD/src/fmod_studio.h>
#include <FMOD/src/fmod_errors.h>
#include <FMOD/src/fmod_common.h>
#include <assert.h>

#include "AudioEngine.hpp"

#include <memory>
#include <iostream>
#include <unordered_map>
#include <math.h>

#include "SoundEffect.hpp"

#include "FMODInstance.hpp"

struct FMODSystem : public AudioEngine
{

    void initialize() override final;
    void update() override final;

    AudioInstance *loadSoundEvent(const char *soundEventName, float volumePlus) override final;
    void clearSoundEvent(const char *pathSound) override final;
    void setVCAvolume(float volume,const char *path) override final;
    void setListener(glm::vec3 pos, glm::vec3 direction, glm::vec3 up) override final;
    AudioInstance *getSoundEvent(const char *pathSound) override final;

private:
    using systemManaged = std::unique_ptr<FMOD_STUDIO_SYSTEM, void (*)(FMOD_STUDIO_SYSTEM *)>;

    FMOD_STUDIO_EVENTDESCRIPTION *eventDescription{nullptr};
    FMOD_STUDIO_EVENTINSTANCE *eventInstance{nullptr};
    FMOD_STUDIO_BANK *masterBank{nullptr};
    FMOD_SYSTEM *coreSystem{nullptr};

    FMOD_RESULT result;

    static void releaseSystem(FMOD_STUDIO_SYSTEM *sys)
    {
        std::cout << "\n*** RELEASE SYSTEM ***\n";
        FMOD_Studio_System_Release(sys);
    }

    inline static systemManaged system{nullptr, releaseSystem};

};
