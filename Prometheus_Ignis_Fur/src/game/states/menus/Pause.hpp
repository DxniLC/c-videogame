#pragma once

#include "../State.hpp"

// Device and Engine
#include <engines/device.hpp>

// Managers
#include <ecs/manager/entitymanager.hpp>

// Timer
#include <util/GameTimer.hpp>

struct MainMenu;
struct PauseState : StateBase
{

    explicit PauseState(StateManager &sm, SGDevice &device)
        : m_states(sm), device{device}, engine{dynamic_cast<SGEngine *>(device.getGraphicEngine())},
          hudEngine{engine->getHUDEngine()}, audioEngine{device.getAudioEngine()},
          renderSystem{engine, nullptr}, inputSystem{device.getInputReceiver()}
    {

        lastCallbackMouse = device.getInputReceiver()->setCursorPosCallback(nullptr);

        device.getInputReceiver()->setVisible(true);

        loadBackground();

        loadElements();
    }

    void update() override final
    {
        // Calculate DeltaTime
        // double currentTime = device.getTime();
        // double deltaTime = currentTime - lastTime;
        // lastTime = currentTime;

        // Update Audio
        audioEngine->update();

        // Update Render
        renderSystem.update(entityManager);

        // Update Input
        inputSystem.update(entityManager);

        // Update Animation
        // animationSystem.update(entityManager, deltaTime);

        // timer to wait 60fps
        timer.waitUntil_ns(NSPF);
    }

    bool alive() override final { return m_alive; }

private:
    bool m_alive{true};
    StateManager &m_states;

    // Device
    SGDevice &device;

    GameTimer timer;

    uint64_t FPS{60};
    uint64_t NSPF{1000000000 / FPS};

    GLFWcursorposfun lastCallbackMouse;

    // Engine
    // TODO: Change SGEngine to abstract
    SGEngine *engine{nullptr};
    HUDEngine *hudEngine{nullptr};
    AudioEngine *audioEngine{nullptr};

    ECS::EntityManager entityManager{};

    // Systems
    RenderMenuSystem<SGEngine> renderSystem;
    InputMenuSystem<ECS::EntityManager> inputSystem;

    HUDElement *background{nullptr};
    HUDElement *controls{nullptr};

    void loadBackground()
    {
        background = hudEngine->addRect({0.5, 0.5}, {1.0, 1.0}, {0, 0, 0, 0.5}, 8);
        controls = hudEngine->addImage("media/textures/Menu/Pause/controles3.png", {0.6, 0.5}, {0.6, 0.6}, 9);
    }

    // LOAD HUD
    void loadElements()
    {
        auto &entity = entityManager.createEntity();

        auto &render = entityManager.addComponent<RenderMenuComponent>(entity);
        render.hudElement = hudEngine->addText("Resume", {1, 1, 1}, {0.1, 0.3}, 40, 9);

        auto &input = entityManager.addComponent<InputMenuComponent>(entity);
        input.actionFunc = [&]()
        {
            m_alive = false;

            device.getInputReceiver()->setCursorPosCallback(lastCallbackMouse);

            device.getInputReceiver()->setCursorPos(0.5, 0.5);

            device.getInputReceiver()->setVisible(false);

            for (auto &render : entityManager.getComponents<RenderMenuComponent>())
                hudEngine->deleteElement(render.hudElement);

            hudEngine->deleteElement(background);
            hudEngine->deleteElement(controls);
        };

        // auto &entity2 = entityManager.createEntity();

        // auto &render2 = entityManager.addComponent<RenderMenuComponent>(entity2);
        // render2.hudElement = hudEngine->addText("Settings", {1, 1, 1}, {0.1, 0.4}, 40, 9);

        auto &entity3 = entityManager.createEntity();

        auto &render3 = entityManager.addComponent<RenderMenuComponent>(entity3);
        render3.hudElement = hudEngine->addText("Exit", {1, 1, 1}, {0.1, 0.5}, 40, 9);

        auto &input2 = entityManager.addComponent<InputMenuComponent>(entity3);
        input2.actionFunc = [&]()
        {
            m_states.replaceState<MainMenu>(m_states, device);
            m_states.eraseStates(2);
        };
    }

    void loadSounds()
    {
    }
};