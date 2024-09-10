
#pragma once

#include <ecs/component/component.hpp>

#include <engines/audio/AudioInstance.hpp>
#include <engines/audio/SoundEffect.hpp>

using SoundEffectManaged = std::unique_ptr<SoundEffect>;

struct AudioComponent : public ECS::ComponentBase<AudioComponent>
{
    explicit AudioComponent(EntityKey e_id) : ECS::ComponentBase<AudioComponent>(e_id){};

    void drop() override final
    {
        for (auto &instance : audioList)
            instance.second->stop();
    }

    // Vector de eventos audio. String con evento, bool de play/stop y puntero al efecto en caso de ser necesario
    std::vector<std::tuple<std::string, bool, SoundEffectManaged>> soundsToUpdate{};

    std::unordered_map<std::string, AudioInstance *> audioList{};

};