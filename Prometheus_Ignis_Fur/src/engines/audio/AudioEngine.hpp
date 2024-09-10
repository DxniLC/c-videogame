#pragma once

// Se debe definir en la clase hija. Por tanto lo declaramos
struct SoundEffect;

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "AudioInstance.hpp"

struct AudioEngine
{

    virtual ~AudioEngine() = default;

    virtual void initialize() = 0;
    virtual void update() = 0;

    virtual AudioInstance *loadSoundEvent(const char *soundEventName, float volumePlus = 0.0f) = 0;
    virtual void clearSoundEvent(const char *pathSound) = 0;
    virtual AudioInstance *getSoundEvent(const char *pathSound) = 0;

    virtual void setVCAvolume(float volume, const char *path) = 0;
    virtual void setListener(glm::vec3 pos, glm::vec3 direction, glm::vec3 up) = 0;

    void cleanAll()
    {
        stopAll();
        audioMaps.clear();
    }

    void stopAll()
    {
        for (auto &pair : audioMaps)
        {
            for (auto &inst : pair.second)
            {
                inst->stop();
            }
        }
    }

protected:
    // using soundManaged = std::unique_ptr<AudioInstance, void (*)(AudioInstance *)>;

    // static void releaseSound(AudioInstance *snd)
    // {
    //     std::cout << "\n*** RELEASE SOUND ***\n";
    //     snd->release();
    // }

    using VectorInstances = std::vector<std::unique_ptr<AudioInstance>>;

    // Mapa donde almacenamos evento y un vector de instancias
    std::unordered_map<std::string, VectorInstances> audioMaps{};
};
