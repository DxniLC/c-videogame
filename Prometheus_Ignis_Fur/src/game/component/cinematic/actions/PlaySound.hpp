#pragma once

#include "CinematicAction.hpp"

#include <util/JSONHelper.hpp>
#include "game/states/menus/components/render.hpp"

struct PlaySound : CinematicAction
{
    explicit PlaySound(Json::Value const &info, AudioEngine *engine)
    {

        path = info["SoundPlay"]["SoundPath"].asString();
        
        sound = engine->loadSoundEvent(path.c_str());
        
    }

    bool update([[maybe_unused]] float const &elapsedTime, [[maybe_unused]] ECS::Entity *entity, [[maybe_unused]] SGEngine *engine, [[maybe_unused]] float const &deltaTime) override final
    {

        if(sound)
            sound->start();

        return true;

       
    }

private:

    std::string path{};
    AudioInstance *sound;
    
};