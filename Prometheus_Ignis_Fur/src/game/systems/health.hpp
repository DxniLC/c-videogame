#pragma once
#include <memory>

template <typename GameCTX>
struct HealthSystem : EventObserver
{
    explicit HealthSystem(EventManager &eventManager);

    void update(GameCTX &g);

    void Process(EventData *eventData) override final;

private:
    bool IsLocked{false};
};
