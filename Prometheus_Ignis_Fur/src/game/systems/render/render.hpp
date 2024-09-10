#pragma once

// #include "debug/debug.hpp"

struct HUDSystem;
struct DebugSystem;
struct GLFWwindow;
template <typename GameCTX, typename Engine>
struct RenderSystem : EventObserver
{
    explicit RenderSystem(Engine *engine, HUDSystem &hud, DebugSystem &debug, EventManager &eventManager);
    void update(GameCTX &g);

    void Process(EventData *data) override final;

private:
    Engine *engine{nullptr};
    HUDSystem *hud{nullptr};
    DebugSystem *debug{nullptr};

    bool StealthMode(bool active);

    static void FocusCallback(GLFWwindow *window, int focused);
};
