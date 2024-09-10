#pragma once

#include "State.hpp"

// Device
#include <engines/device.hpp>

// Engines
// Include on main menu

// Managers
#include <ecs/manager/entitymanager.hpp>
#include <game/manager/event/EventManager.hpp>
#include <game/manager/puzzle/PuzzleManager.hpp>

// Systems
#include <game/systems/entity.tpp>
#include <game/systems/render/render.tpp>
#include <game/systems/audio.tpp>
#include <game/systems/input.tpp>
#include <game/systems/physics.tpp>
#include <game/systems/camera.tpp>
#include <game/systems/raycast.tpp>
#include <game/systems/collision.tpp>
#include <game/systems/event.hpp>
#include <game/systems/interaction.tpp>
#include <game/systems/puzzle.tpp>
#include <game/systems/animation.tpp>
#include <game/systems/render/HUD/HUD.hpp>
#include <game/systems/health.tpp>
#include <game/systems/sensory.tpp>
#include <game/systems/cinematic.tpp>
#include <game/systems/achievement.tpp>
#include <game/systems/AI.tpp>
#include <game/systems/powerup.tpp>

#include "menus/Pause.hpp"

struct GameManager : StateBase, EventObserver
{
    explicit GameManager(StateManager &sm, SGDevice &device, ECS::EntityManager &entityManager, PuzzleManager &puzzleManager, EntityKey playerKey)
        : m_states(sm), device{device}, lastTime{device.getTime()}, engine{dynamic_cast<SGEngine *>(device.getGraphicEngine())}, audioEngine{device.getAudioEngine()},
          entityManager{entityManager}, puzzleManager{puzzleManager}, eventManager{entityManager.getSingletonComponent<EventManager>()},
          debugSystem{&device}, hudSystem{engine->getHUDEngine(), engine, *eventManager}, renderSystem{engine, hudSystem, debugSystem, *eventManager},
          audioSystem{audioEngine, *eventManager}, inputSystem{device.getInputReceiver(), *eventManager},
          eventSystem{*eventManager}, interactSystem{*eventManager}, puzzleSystem{*eventManager},
          healthSystem{*eventManager}, sensorySystem{*eventManager}, cinematicSystem{*eventManager, engine}, aiSystem{*eventManager}
    {

        audioSystem.setPlayer(playerKey);

        // SUBSCRIBE EVENTS
        eventManager->EventQueueInstance.Subscribe(EventIds::Pause, this);
        eventManager->EventQueueInstance.Subscribe(EventIds::LoadLevel, this);

        engine->enableSGOptions(SG_FRUSTUM_CULLING);
        engine->enableSGOptions(SG_TILE_OPTIMIZATION);

        engine->update(1.0f);
    }

    // Release all and save game
    ~GameManager()
    {
        device.getInputReceiver()->clearCallbacks();
        engine->disableSGOptions(SG_FRUSTUM_CULLING);
        engine->disableSGOptions(SG_TILE_OPTIMIZATION);
    }

    void update() override final
    {

        // LIMITAMOS A 60FPS
        timer.waitUntil_ns(NSPF);
        timer.start();

        // Game Loop
        double currentTime = device.getTime();
        float frameTime = (IsPause) ? constDeltaTime : float(currentTime - lastTime);
        IsPause = false;

        // update current time to new time
        lastTime = currentTime;

        accumulator += frameTime;

        if (debugSystem.profilingSystems)
            measuringTimes();
        else
        {

            entitySystem.update(entityManager);

            renderSystem.update(entityManager);
            audioSystem.update(entityManager, frameTime);

            // Si input no esta aqui, varia la velocidad de juego
            cinematicSystem.update(entityManager, frameTime);

            inputSystem.preupdate(entityManager);

            cameraSystem.update(entityManager, frameTime);

            puzzleSystem.preupdate(entityManager, puzzleManager);
            animationSystem.update(entityManager, frameTime);

            aiSystem.update(entityManager, frameTime);

            while (accumulator >= constDeltaTime)
            {
                // Actualizamos la fuerza el numero de veces que se aplica el loop constante

                inputSystem.update(entityManager);
                physicsSystem.update(entityManager, constDeltaTime);
                engine->update(constDeltaTime);
                sensorySystem.update(entityManager);

                raycastSystem.update(entityManager);
                eventSystem.updateTriggers(entityManager);

                collisionSystem.update(entityManager);
                healthSystem.update(entityManager);

                eventSystem.updateTriggers(entityManager);

                engine->update(constDeltaTime);

                accumulator -= constDeltaTime;
            }

            interactSystem.update(entityManager);
            puzzleSystem.update(entityManager, puzzleManager);

            powerUpSystem.update(entityManager);

            achievementSystem.update(entityManager);

            eventSystem.lateUpdate();
        }
    }

    void Process(EventData *eventData) override final
    {
        if (eventData->EventId == EventIds::Pause)
        {
            AddPauseState();
        }
        else if (eventData->EventId == EventIds::LoadLevel)
        {
            m_alive = false;
        }
    }

    bool alive() override final
    {
        return m_alive;
    }

private:
    bool m_alive{true};
    StateManager &m_states;

    // Device window
    SGDevice &device;

    bool IsPause{false};

    // Consts
    const float constDeltaTime{1.0f / 60.0f};
    double lastTime{};
    double accumulator{0};

    uint64_t FPS{60};
    uint64_t NSPF{1000000000UL / FPS};

    GameTimer timer;

    // Engines
    // TODO: Change SGEngine to abstract
    SGEngine *engine{nullptr};
    AudioEngine *audioEngine{nullptr};

    // Managers
    ECS::EntityManager &entityManager; // Initialized on LevelManager
    PuzzleManager &puzzleManager;      // Initialized on LevelManager
    EventManager *eventManager{nullptr};

    // Systems
    EntityManagerSystem<ECS::EntityManager> entitySystem{};
    DebugSystem debugSystem;
    HUDSystem hudSystem;
    RenderSystem<ECS::EntityManager, SGEngine> renderSystem;
    AudioSystem<ECS::EntityManager> audioSystem;
    PhysicsSystem<ECS::EntityManager> physicsSystem{};
    InputSystem<ECS::EntityManager> inputSystem;
    CameraSystem<ECS::EntityManager> cameraSystem{};
    RaycastSystem<ECS::EntityManager> raycastSystem{};
    CollisionSystem<ECS::EntityManager> collisionSystem{};
    EventSystem eventSystem;
    InteractionSystem<ECS::EntityManager> interactSystem;
    PuzzleSystem<ECS::EntityManager> puzzleSystem;
    AnimationSystem<ECS::EntityManager> animationSystem{};
    HealthSystem<ECS::EntityManager> healthSystem;
    SensorySystem<ECS::EntityManager> sensorySystem;
    AchievementSystem<ECS::EntityManager> achievementSystem{};
    PowerUpSystem<ECS::EntityManager> powerUpSystem{};

    CinematicSystem<ECS::EntityManager> cinematicSystem;
    
    AISystem<ECS::EntityManager> aiSystem;


    inline static auto timedCall = [](std::string_view name, auto &&func)
    {
        GameTimer timer;
        func();
        std::cout << " [" << name << "-" << timer.ellapsedDuration<ChronoTime::Nano>() << "]";
        // std::cout << timer.ellapsed() << ";";
    };

    void measuringTimes()
    {
        std::cout << "\n\n**** INIT_FRAME ****\n";
        timedCall("REN", [&]()
                  { renderSystem.update(entityManager); });
        timedCall("AUD", [&]()
                  { audioSystem.update(entityManager, constDeltaTime); });

        timedCall("INP", [&]()
                  { inputSystem.update(entityManager); });
        timedCall("CAM", [&]()
                  { cameraSystem.update(entityManager, constDeltaTime); });

        timedCall("ENT", [&]()
                  { entitySystem.update(entityManager); });

        timedCall("PRE-PZL", [&]()
                  { puzzleSystem.preupdate(entityManager, puzzleManager); });
        timedCall("ANIM", [&]()
                  { animationSystem.update(entityManager, constDeltaTime); });

        std::cout << "\n\n---- WHILE DELTATIME LOOP ----\n";
        while (accumulator >= constDeltaTime)
        {
            timedCall("PHY", [&]()
                      { physicsSystem.update(entityManager, constDeltaTime); });
            // Una solucion puede ser hacer que se recorra los arboles ahcia arriba para aquellas cosas que se hayan modificado
            // Recorrer todo el arbol para obtener la malla calculada para la colision
            timedCall("TREE_1", [&]()
                      { engine->update(constDeltaTime); });

            // Mirar si podemos sacarlo fuera ya que sirve para lanzar un rallo
            timedCall("SENS", [&]()
                      { sensorySystem.update(entityManager); });

            timedCall("RAY", [&]()
                      { raycastSystem.update(entityManager); });
            timedCall("EVENT_RAY", [&]()
                      { eventSystem.updateTriggers(entityManager); });

            timedCall("COLL", [&]()
                      { collisionSystem.update(entityManager); });
            timedCall("HP", [&]()
                      { healthSystem.update(entityManager); });

            timedCall("EVENT_RAY_2", [&]()
                      { eventSystem.updateTriggers(entityManager); });

            // Recorrer todo el arbol para obtener malla final despues de comprobar colisiones y aplicar correcciones
            timedCall("TREE_2", [&]()
                      { engine->update(constDeltaTime); });

            accumulator -= constDeltaTime;
        }

        std::cout << "\n\n------------------------------\n";

        timedCall("INT", [&]()
                  { interactSystem.update(entityManager); });
        timedCall("PUZL", [&]()
                  { puzzleSystem.update(entityManager, puzzleManager); });
    }

    bool AddPauseState()
    {
        IsPause = true;
        std::cout << "\nENTRO A PAUSE STATE\n";
        m_states.addState<PauseState>(m_states, device);
        return false;
    }
};