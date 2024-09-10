#pragma once

#include <game/component/audio/effects/lowpass.hpp>

#include <game/component/audio/effects/reverb.hpp>

#include <memory>

struct EventManager;

template <typename GameCTX>
struct AudioSystem : EventObserver
{
    explicit AudioSystem(AudioEngine *audioEngine, EventManager &eventManager);
    void update(GameCTX &g, float const &deltaTime);
    void setPlayer(EntityKey e_key)
    {
        this->player_key = e_key;
    }

    void Process(EventData *eventData) override final;

private:
    AudioEngine *audioEngine{nullptr};
    EntityKey player_key;

    SoundEffectManaged effectLowPass{std::make_unique<LowPassEffect>(1000.0f)};
    SoundEffectManaged effectReverb{nullptr};
    bool IsStealthMode{false};
    bool CleanStealth{false};
    bool IsOnCave{false};
    bool CleanReverb{false};
    const float speedToStealth{100.0f};
};
