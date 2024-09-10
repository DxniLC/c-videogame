
#include "audio.hpp"

#include <game/component/audio/audio.hpp>

#include <game/manager/event/EventManager.hpp>

template <typename GameCTX>
AudioSystem<GameCTX>::AudioSystem(AudioEngine *audioEngine, EventManager &eventManager) : audioEngine{audioEngine}
{
    eventManager.EventQueueInstance.Subscribe(EventIds::StealthMode, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::OnCave, this);
}

template <typename GameCTX>
void AudioSystem<GameCTX>::update(GameCTX &g, [[maybe_unused]] float const &deltaTime)
{

    auto *playerEntity = g.getEntityByKey(this->player_key);
    auto *renderplayer = playerEntity->template getComponent<RenderComponent>();

    auto *camaraNodo = renderplayer->getNodeChild(RenderComponent::RenderType::CAMERA);
    glm::vec3 front{};
    glm::vec3 up{};
    if (camaraNodo)
    {
        auto *camEntity = camaraNodo->template getEntity<TCamera>();
        front = camEntity->Front;
        up = camEntity->Up;
    }
    glm::vec3 posplayer = renderplayer->node->globalPosition;

    audioEngine->setListener(posplayer, front, up);

    AudioInstance *soundEvent = audioEngine->getSoundEvent("Music/ambient");
    if (soundEvent)
        soundEvent->setParameterbyName("ModoCombate", 2);

    // Primer parametro es el ajuste de volumen es decir si en FMOD tu volumen es de 10 en 1 es 10 el volumen , si vas aumentando , el numero que pongas se multiplica por tu valor , es decir con parametro 2 es el doble y si pone 0.5 es la mitad
    //  es decir cuando se haga el slider de subit o bajar el voluemn el valor oscilara entre 0-1-2 por ejemplo
    // Segundo parametro es el nombre del VCA
    audioEngine->setVCAvolume(1, "Music");

    for (auto &audio : g.template getComponents<AudioComponent>())
    {
        auto *audioEntity = g.getEntityByKey(audio.getEntityKey());

        auto *render = audioEntity->template getComponent<RenderComponent>();
        glm::vec3 directionaudio{1.0f};
        if (render)
            directionaudio = glm::normalize(posplayer - render->node->globalPosition);

        // TODO: Mejorar con el sistema de eventos con LISTENER. Ahi es donde se hará qu epare tambien y no hace falta tener en audioPlayer el lowpass

        if (IsOnCave || IsStealthMode)
        {
            for (auto &mapInstances : audio.audioList)
            {
                auto *instance = mapInstances.second;

                if (instance->IsPlaying())
                {
                    if (IsOnCave)
                        effectReverb->update(instance);
                    if (IsStealthMode)
                        effectLowPass->update(instance);
                }
            }
        }

        for (auto &sound : audio.soundsToUpdate) // Para entidades que estan quietas o sonidos de tipo SFX que provienen de un sitio y no cambia el sitio del que provienen
        {
            auto const &soundName = std::get<std::string>(sound);
            auto const &stateSound = std::get<bool>(sound);
            auto &effect = std::get<SoundEffectManaged>(sound);

            auto &audioInstance = audio.audioList[soundName];
            if (stateSound)
            {
                // Si no se ha iniciado
                if (audio.getEntityKey() != this->player_key)
                {
                    audioInstance->set3DPosition(render->node->globalPosition, directionaudio);
                }
                if (effect.get())
                {
                    effect->update(audioInstance);
                }
                audioInstance->start();
            }
            else
            {
                audioInstance->stop();
            }
        }
        audio.soundsToUpdate.clear();
    }

    if (CleanStealth)
    {
        CleanStealth = false;
        for (auto &audio : g.template getComponents<AudioComponent>())
        {
            for (auto &mapInstances : audio.audioList)
            {
                auto *instance = mapInstances.second;
                effectLowPass->clearEffect(instance);
            }
        }
    }

    audioEngine->update();
}

template <typename GameCTX>
void AudioSystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::StealthMode)
    {

        StealthData *data{dynamic_cast<StealthData *>(eventData)};
        if (data)
        {
            if (not effectLowPass)
                effectLowPass = std::make_unique<LowPassEffect>(500.0f);

            IsStealthMode = data->active;
            CleanStealth = not data->active;
        }
        else
            std::cout << "\n***** ERROR StealthEvent Audio ******\n";
    }
    else if (eventData->EventId == EventIds::OnCave)
    {
        CaveData *data{dynamic_cast<CaveData *>(eventData)};
        if (data)
        {
            if (data->status)
                effectReverb = std::make_unique<ReverbEffect>(data->level);
            else
                effectReverb = nullptr;
            IsOnCave = data->status;
        }
    }
}
