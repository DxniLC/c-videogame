#pragma once

#include "State.hpp"

#include <thread>

// Device and Engine
#include <engines/device.hpp>

// Managers
#include <ecs/manager/entitymanager.hpp>
#include <game/factory/objectFactory.tpp>
#include <game/manager/puzzle/PuzzleManager.hpp>
#include <game/manager/animation/AnimationManager.hpp>

// Timer
#include <util/GameTimer.hpp>

// Singleton Components
#include <game/manager/AchievementManager.hpp>

#include "GameManager.hpp"
#include <game/data/GameDataManager.hpp>

struct LevelManager : StateBase, EventObserver
{

    explicit LevelManager(StateManager &sm, SGDevice &device)
        : m_states(sm), device{device}, lastTime{device.getTime()},
          engine{dynamic_cast<SGEngine *>(device.getGraphicEngine())},
          hudEngine{engine->getHUDEngine()}, audioEngine{device.getAudioEngine()},
          factory{entityManager, engine, audioEngine, device.getInputReceiver(), &puzzleManager}
    {
        device.getInputReceiver()->setVisible(false);

        entityManager.addSingletonComponent<EventManager>();
        entityManager.addSingletonComponent<AnimationManager>();

        initDataGame = dataManager.LoadGame();
        LoadNewLevel(initDataGame["Level"].asString());

        // Singleton components
        auto &achievementManager = entityManager.addSingletonComponent<AchievementManager>();
        achievementManager.readAchievementFile(dataManager.getAchievementPath());
    }

    ~LevelManager()
    {

        auto *achievementManager = entityManager.getSingletonComponent<AchievementManager>();
        if (achievementManager)
        {
            auto const &dataValue = achievementManager->SaveAchievementsOnFile();
            dataManager.UploadOnWeb(dataValue);
        }
    }

    void update() override final
    {

        if (new_level_update)
        {
            SaveGame();
            LoadNewLevel(currentLevel);
            new_level_update = false;
        }

        // Calculate DeltaTime
        // double currentTime = device.getTime();
        // double deltaTime = currentTime - lastTime;
        // lastTime = currentTime;

        // Load Level Resources
        // ............
        auto entitiesLoaded = factory.loadResourcesxFragments(entityManager);

        // Set Progress Bar Value * deltaTime
        float progressPercent = (float(entitiesLoaded) / float(TotalElementsToLoad)) * 100.f;

        updateProgressBar(progressPercent);

        // Update Audio
        audioEngine->update();

        // Update Render
        engine->beginScene();
        engine->drawUI();
        engine->endScene();

        if (entitiesLoaded >= TotalElementsToLoad)
        {

            // m_alive = false;
            // Clear Resource
            hudEngine->clearAll();
            auto path = factory.getCurrentBackground(currentLevel);
            engine->clearResource(path.c_str()); // CleanBackGround

            factory.loadCinematicData();

            factory.loadTiles();

            factory.clean();

            // Add Game State

            m_states.addState<GameManager>(m_states, device, entityManager, puzzleManager, factory.getPlayerKey());
        }

        // timer to wait 60fps
        timer.waitUntil_ns(NSPF);
    }

    bool alive() override final { return m_alive; }

    void Process(EventData *eventData) override final
    {
        if (eventData->EventId == EventIds::LoadLevel)
        {
            LoadLevelData *data{dynamic_cast<LoadLevelData *>(eventData)};
            new_level_update = true;

            auto *achieveManager = entityManager.getSingletonComponent<AchievementManager>();
            if (achieveManager)
            {
                achieveManager->achievementData.lastlevel = currentLevel;
            }

            currentLevel = data->levelName;
        }
    }

private:
    bool m_alive{true};
    StateManager &m_states;

    // Device
    SGDevice &device;

    std::string currentLevel{""};
    bool new_level_update{false};

    GameTimer timer;

    uint64_t FPS{60};
    uint64_t NSPF{1000000000 / FPS};

    double lastTime{};

    int TotalElementsToLoad{0};

    // Engine
    // TODO: Change SGEngine to abstract
    SGEngine *engine{nullptr};
    HUDEngine *hudEngine{nullptr};
    AudioEngine *audioEngine{nullptr};

    GameDataManager dataManager{};

    Json::Value initDataGame{};

    // Managers
    ECS::EntityManager entityManager{};
    PuzzleManager puzzleManager{};
    GameObjectFactory<ECS::EntityManager, SGEngine> factory;

    std::vector<std::string> phrases = {
        "¿Te imaginas un ser divino que viaje a velocidades sobrenaturales y transmita mensajes entre los dioses y los mortales?. Ese es Hermes, el mensajero divino de la mitologia griega.",
        "¿Te has maravillado alguna vez con la musica celestial? Apolo, el dios de la musica y la poesia, era el responsable de su creacion y deleitaba a todos con su talento unico",
        "El legendario heroe Hercules, conocido por su fuerza sobrehumana, tuvo que realizar doce trabajos como parte de su penitencia. Estas hazañas incluyeron enfrentarse a criaturas miticas como el Leon de Nemea y la Hidra de Lerna.",
        "Segun la mitologia griega, los centauros eran criaturas mitad humanas y mitad caballos. Eran conocidos por su ferocidad en la batalla y su amor por el vino.",
        "El mito del laberinto de Creta cuenta la historia del astuto rey Minos y el temible Minotauro, una criatura mitad hombre y mitad toro.",
        "¿Sabias que en el monte Olimpo, residencia de los dioses griegos, se celebraban grandes festivales y banquetes divinos?. Se decia que el nectar y la ambrosia, alimentos de los dioses, concedian inmortalidad a quienes los consumian.",
        "¿Sabias que Zeus, el poderoso rey de los dioses en la mitologia griega, es conocido por desatar su ira a traves de relampagos y truenos?",
        "Antiguamente, los marineros confiaban en la proteccion del poderoso Poseidon, el dios de los mares y los terremotos, para sobrevivir a las turbulentas aguas y a los caprichos del oceano.",
        "¿Sabias que existian las Harpias? Estas criaturas con cuerpo de ave y rostro de mujer aterrorizaban a los mortales robando su comida. Se dice que eran enviadas por los dioses como castigo.",
        "¿Conoces la historia de Pandora? Segun la mitologia, Pandora fue creada por los dioses como una venganza hacia los hombres. Le dieron una caja y le dijeron que no la abriera, pero su curiosidad la llevo a liberar todos los males y enfermedades en el mundo.",
    };

    void InitEvents(EventManager &eventManager)
    {
        eventManager.EventQueueInstance.Subscribe(EventIds::LoadLevel, this);
    }

    HUDImage *prometeo{nullptr};
    HUDRect *progress{nullptr};

    void loadBackground()
    {
        std::string getCurrentBackground(std::string levelName);
        auto path = factory.getCurrentBackground(currentLevel);
        hudEngine->addImage(path.c_str(), {0.5, 0.5}, {1, 1}, -1);

        progress = hudEngine->addRect({0.1, 0.8}, {0.0, 0.03}, {0.08, 0.18, 0.74, 0.5}, 0, false);

        prometeo = hudEngine->addImage("media/textures/Menu/Load/Prometeo_Load1.png", {0.1, 0.77}, {130, 140}, 1);
        std::vector<ResourceTexture *> sprites = {
            engine->getTexture("media/textures/Menu/Load/Prometeo_Load1.png"),
            engine->getTexture("media/textures/Menu/Load/Prometeo_Load2.png"),
            engine->getTexture("media/textures/Menu/Load/Prometeo_Load3.png"),
            engine->getTexture("media/textures/Menu/Load/Prometeo_Load2.png"),

        };

        prometeo->setAnimationSprite(0, sprites, 0.2f, true);
        prometeo->playAnimation(0);
    }

    void loadText()
    {
        int randomIndex = std::rand() % int(phrases.size());
        std::string &selectedPhrase = phrases[randomIndex];

        auto *text = hudEngine->addText(selectedPhrase.c_str(), {1, 1, 1}, {0.5, 0.88f}, 22, 2);
        text->setLimits(0.8f, 22);
    }

    void loadMusic()
    {
    }

    void updateProgressBar(float currentProgress)
    {
        prometeo->position.x = 0.1f + currentProgress * (0.9f - 0.1f) / 100.f;
        progress->size.x = currentProgress * (0.9f - 0.1f) / 100.f;
    }

    void SaveGame()
    {

        SaveData save{};
        save.level = currentLevel;
        auto *player = entityManager.getEntityByKey(factory.getPlayerKey());
        if (player)
        {
            auto *abilities = player->template getComponent<AbilityComponent>();
            if (abilities)
                save.abilities = int(abilities->abilitySize);
            auto *powers = player->template getComponent<PowerUpComponent>();
            if (powers)
            {
                for (auto &power : powers->powerUps)
                    save.powers.emplace_back(power->powerID);
            }
        }
        dataManager.SaveGame(save);
    }

    void LoadNewLevel(std::string newLevel)
    {
        currentLevel = newLevel;

        // Limpiamos toda la Escena (Camaras, Luces, HUD, Nodos)
        audioEngine->cleanAll();

        engine->clearScene();

        engine->initScene();

        device.getInputReceiver()->setVisible(false);

        auto *animationManager = entityManager.getSingletonComponent<AnimationManager>();
        if (animationManager)
            animationManager->clearAnimations();

        auto *eventManager = entityManager.getSingletonComponent<EventManager>();
        assert(eventManager);

        eventManager->cleanAll();

        // Cargamos la imagen de fondo
        loadBackground();

        // Cargamos los textos que pueden aparecer
        loadText();

        // Cargamos sonidos de la pantalla de carga
        loadMusic();

        // Inicializamos eventos
        InitEvents(*eventManager);

        factory.InitEvents(*eventManager);

        TotalElementsToLoad = factory.initLevel(currentLevel.c_str(), *eventManager);

        factory.loadSaveData(dataManager.LoadGame());
    }
};