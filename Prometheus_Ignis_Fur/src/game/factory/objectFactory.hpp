
#pragma once

#include <string>
#include <vector>
#include <util/JSONHelper.hpp>

#include <optional>

#include <ecs/util/typealiases.hpp>

#include <game/manager/event/EventManager.hpp>
#include <game/factory/AIFactory.hpp>

#include <game/manager/ObjectsTypes.hpp>
#include <game/component/powerup/PowerUpId.hpp>

namespace ECS
{
    struct EntityManager;
    struct Entity;
}

struct InputReceiver;

struct PuzzleManager;

struct RenderComponent;
struct PuzzleManager;

struct AudioEngine;

struct CinematicAction;

struct Ability;
struct PowerUp;

struct TBillboard;

template <typename GameCTX, typename Engine>
struct GameObjectFactory : EventObserver
{
    explicit GameObjectFactory(GameCTX &em, Engine *eng, AudioEngine *audioEngine, InputReceiver *inp, PuzzleManager *puzzleManager);

    ECS::Entity &createPlayer();
    ECS::Entity &createCamera(ECS::Entity *e_focused = nullptr) const;

    ECS::Entity &createArrow(glm::vec3 position) const;

    ECS::Entity &createDivineObject(glm::vec3 position, glm::vec3 scale, std::string pathObject) const;

    ECS::Entity &createProjectile(glm::vec3 position, glm::vec3 direction, float inputForce, bool hasGravity, bool hasFriction) const;

    ECS::Entity &createBoxPuzzle(glm::vec3 position) const;

    ECS::Entity &createPlatform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 direction, float velocity, float maxRange, bool loop, std::string meshPath = "media/models/Elements/platform.obj") const;

    ECS::Entity &createDoor(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    ECS::Entity &createPlate(glm::vec3 position, glm::vec3 rotation) const;

    ECS::Entity &createFloor(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension, float friction = 0.2f, float bounciness = 0.0f, bool defaultTexture = false) const;

    ECS::Entity &createFloorNPC(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension, float friction = 0.2f, float bounciness = 0.0f, bool defaultTexture = false) const;

    ECS::Entity &createVoid(glm::vec3 position, glm::vec3 rotation) const;

    ECS::Entity &createCheckPoint(glm::vec3 position, glm::vec3 dimension) const;

    ECS::Entity &createWallSpikes(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 direction, float velocity, float range) const;

    ECS::Entity &createChest(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string chestContain) const;

    ECS::Entity &createEventChangeLevel(std::string LevelLoad, glm::vec3 position, glm::vec3 dimension) const;

    ECS::Entity &createClimbingWall(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension) const;

    void ThrowBall(glm::vec3 position, glm::vec3 direction, float force, float range) ;



    void createBillboard4Interaction(const char *path, glm::vec3 position, glm::vec2 size, glm::vec3 locked_axis, EntityKey EntityAsociated) const;

    TBillboard *createBillboard4Tutorial(glm::vec3 position, glm::vec3 dimension, glm::vec3 positionBoard, glm::vec2 dimensionBoard, std::string pathBoard) const;

    void addSnowEffect(TNode *camera);

    [[nodiscard]] constexpr EntityKey getPlayerKey() const noexcept { return PlayerKey; };

    void InitEvents(EventManager &eventManager);

    int initLevel(const char *pathJson, EventManager &eventManager);

    void loadSaveData(Json::Value loadData);

    void clean();

    int loadResourcesxFragments(GameCTX &g);

    void removeEntity(EntityKey key);

    void loadTiles();
    void loadCinematicData();

    void Process(EventData *eventData) override final;

    void CreatePrueba(glm::vec3 pos, glm::vec3 recollectBalls);
    void CreateSeeAndFollow(glm::vec3 pos);
    void CreatePatrollAndPathfinding(glm::vec3 pos, std::vector<glm::vec3>);
    void CreateHearAndFly(glm::vec3 pos);

    std::string getCurrentBackground(std::string levelName);

    void GeneratePathfinding(std::string mapPath);

private:
    GameCTX *entityManager{nullptr};
    Engine *engine{nullptr};
    AudioEngine *audioEngine{nullptr};
    InputReceiver *inputReceiver{nullptr};
    PuzzleManager *puzzleManager{nullptr};

    AIFactory aiFactory{};

    ResourceShader *defaultShader{nullptr};

    std::vector<std::string> skybox_path{};

    std::unordered_map<std::string, std::pair<ObjectType, std::string>> divineObjectPath = {
        {"cuerno", std::make_pair(ObjectType::CUERNO_DIVINE, "media/models/Elements/cuernolow.obj")},
        {"botas", std::make_pair(ObjectType::CUERNO_DIVINE, "media/models/Elements/botas.obj")},
    };

    std::unordered_map<std::string, ObjectType> powerUpsTypes = {
        {"JumpPower", ObjectType::POWER_JUMP},
        {"SpeedPower", ObjectType::POWER_SPEED},
    };

    std::unordered_map<std::string, std::string> tutorialBoard = {
        {"MoveTutorial", "media/textures/Billboard/wasd.png"},
        {"RunTutorial", "media/textures/Billboard/CORRER.png"},
        {"JumpTutorial", "media/textures/Billboard/SALTO.png"},
        {"DashTutorial", "media/textures/Billboard/DASH.png"},
        {"DobleJumpTutorial", "media/textures/Billboard/DOBLE_SALTO.png"},
        
    };

    std::unordered_map<std::string, std::string> backgroundLevel = {
        {"Tutorial", "media/textures/Menu/BackgroundLevels/foto8.png"},
        {"Inicio", "media/textures/Menu/BackgroundLevels/11.png"},
        {"Nivel1", "media/textures/Menu/BackgroundLevels/foto26.png"},
        {"Nivel2", "media/textures/Menu/BackgroundLevels/foto59.png"},
        {"Nivel3", "media/textures/Menu/BackgroundLevels/15.png"},
    };

    Json::Value levelManager;
    Json::Value currentLevelData;
    Json::Value::iterator jsonItr;
    std::size_t NumElementsToLoad{0};
    int elementsLoaded{0};
    std::unordered_map<std::string, std::function<void(Json::Value const &element)>> FactoryFunction;

    EntityKey SceneKey;
    EntityKey PlayerKey;
    std::string currentLevelName{};

    // Obtener Cinematica del Nivel Actual
    std::unordered_map<std::string, std::function<std::unique_ptr<CinematicAction>(Json::Value const &info)>> ActionsMap;

    // Tiles
    std::vector<glm::vec4> LevelTiles{};

    std::map<int, EntityKey> cinematicsEvents{};
    std::string curentCinematicFile{};

    void InitFactoryFunctions();
    void InitCinematicFunctions();

    void setPlatformLevel(Json::Value const &element, std::string meshPath);

    std::vector<std::function<std::unique_ptr<Ability>()>> abilities{};

    std::unordered_map<PowerUpId, std ::function<std::unique_ptr<PowerUp>()>> powerUps{};
};