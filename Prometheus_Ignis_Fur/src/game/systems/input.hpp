#pragma once
#include <memory>
#include <chrono>
#include <engines/input.hpp>

struct EventManager;

template <typename GameCTX>
struct InputSystem : EventObserver
{
    explicit InputSystem(InputReceiver *rec, EventManager &eventManager);

    void preupdate(GameCTX &g);
    bool update(GameCTX &g);

    void Process(EventData* eventData) override final;

private:
    InputReceiver *receiver{nullptr};
    bool IsLocked{false};
    bool LockSystem(bool status);
    GLFWcursorposfun prevCursorFunc{nullptr};
};
