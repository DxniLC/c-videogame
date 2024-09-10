#pragma once

template <typename GameCTX>
struct AISystem : EventObserver
{
    explicit AISystem(EventManager &man);
    void update(GameCTX &g, float const &deltaTime);

    void Process(EventData *eventData) override final;

};
