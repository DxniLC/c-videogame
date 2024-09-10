#include "objectFactory.hpp"

#include <ecs/manager/entitymanager.hpp>

#include <game/manager/puzzle/PuzzleManager.hpp>

#include <engines/graphic/SGEngine/SGEngine.hpp>
#include <engines/input.hpp>
#include <engines/audio/AudioEngine.hpp>

// particulas
#include <game/particles/FireParticle.hpp>
#include <game/particles/DirtParticle.hpp>
#include <game/particles/SnowParticle.hpp>

#include <random>

// Components
#include <game/component/render.hpp>
#include <game/component/audio/audio.hpp>
#include <game/component/physics.hpp>
#include <game/component/input/input.hpp>
#include <game/component/camera/camera.hpp>
#include <game/component/body/body.hpp>
#include <game/component/body/worldbody.hpp>
#include <game/component/body/scenebody.hpp>
#include <game/component/raycast/raycast.hpp>
#include <game/component/frontbox.hpp>
#include <game/component/ability/ability.hpp>
#include <game/component/meshAnimation.hpp>
#include <game/component/animation.hpp>
#include <game/component/inventory.hpp>
#include <game/component/object.hpp>
#include <game/component/checkpoint.hpp>
#include <game/component/health.hpp>
#include <game/component/sensory.hpp>
#include <game/component/autodestroy.hpp>
#include <game/component/event.hpp>
#include <game/component/cinematic/cinematic.hpp>
#include <game/component/AI.hpp>
#include <game/component/powerup/powerup.hpp>

#include <game/AI/Blackboard.hpp>

// Animations
#include <game/animations/TranslateAnimation.hpp>

// Puzzles
#include <game/puzzles/Plate.hpp>

// EventData
#include <game/events/eventsData/ProjectileEventData.hpp>
#include <game/events/eventsData/ShowBoardData.hpp>
#include <game/events/eventsData/CaveData.hpp>
#include <game/events/eventsData/AchievementEventData.hpp>
#include <game/events/eventsData/PowerUpData.hpp>
#include <game/events/eventsData/ThrowBall.hpp>

#include <fstream>

template <typename GameCTX, typename Engine>
GameObjectFactory<GameCTX, Engine>::GameObjectFactory(GameCTX &em, Engine *eng, AudioEngine *audioEngine, InputReceiver *inp, PuzzleManager *puzzleManager)
    : entityManager{&em}, engine{eng}, audioEngine{audioEngine}, inputReceiver{inp}, puzzleManager{puzzleManager}
{

    std::ifstream file("media/levels/Manager.json");
    if (!file)
        throw std::runtime_error("No se puede abrir el fichero JSON\n");

    Json::Reader reader;
    reader.parse(file, levelManager);
    file.close();

    defaultShader = eng->getShader("media/shaders/VertexShader.vert", "media/shaders/Toon/Toon_Outline.frag", true);

    // Cinematic Actions

    InitCinematicFunctions();

    InitFactoryFunctions();

    abilities = {
        []()
        { return std::make_unique<DashAbility>(); },
        []()
        { return std::make_unique<DoubleJumpAbility>(); },
    };

    powerUps = {
        {PowerUpId::MOREJUMP, []()
         { return std::make_unique<MoreJump>(); }},
        {PowerUpId::MORESPEED, []()
         { return std::make_unique<MoreSpeed>(); }},
    };

    InputControl::setReceiver(inp);
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::InitEvents(EventManager &eventManager)
{
    eventManager.EventQueueInstance.Subscribe(EventIds::CreateProjectile, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::ShowControlBoard, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::ShowAchievement, this);
    eventManager.EventQueueInstance.Subscribe(EventIds::ThrowBall, this);
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createPlayer()
{
    auto &entity = entityManager->createEntity();
    PlayerKey = entity.getEntityKey();

    entity.type = EntityType::PLAYER;

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createAnimatedModel("media/models/Characters/Player.dae", engine->getShader("media/shaders/SkeletalShader.vert", "media/shaders/Toon/Toon_Outline.frag", true));
    render.node->localTransform.setPosition({0, 5, 0});
    render.node->localTransform.setRotate({0, 180, 0});

    entityManager->template addComponent<PhysicsComponent>(entity);

    auto &input = entityManager->template addComponent<InputComponent>(entity);
    input.receiver = inputReceiver;
    input.controlType = std::make_unique<PlayerDefault>(render.node);

    auto *boundingBoxNode = engine->createBoundingBox(render.node, Transform(), glm::vec3(0.6f, 2.1f, 0.4f));
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.20f);

    auto &front = entityManager->template addComponent<FrontBoxComponent>(entity);
    front.range = 1;

    entityManager->template addComponent<AbilityComponent>(entity);

    entityManager->template addComponent<InventoryComponent>(entity);

    auto &health = entityManager->template addComponent<HealthComponent>(entity);
    health.HP = 50;

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;
    checkpoint.spawnLevel.second = health.HP;

    checkpoint.checkpointSpawn = checkpoint.spawnLevel;

    auto &sensory = entityManager->template addComponent<SensoryComponent>(entity);
    sensory.RangeSight = 0;

    auto &animation = entityManager->template addComponent<MeshAnimationComponent>(entity);
    animation.animationEntity = render.node->template getEntity<TAnimatedMesh>();
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_IDLE, engine->getAnimation("media/animation/Player_Idle.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_MOVE, engine->getAnimation("media/animation/Player_Walk.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_CLIMB, engine->getAnimation("media/animation/Player_Climb.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_RUN, engine->getAnimation("media/animation/Player_Run.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_JUMP, engine->getAnimation("media/animation/Player_Jump.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_D_JUMP, engine->getAnimation("media/animation/Player_D_Jump.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_FALL, engine->getAnimation("media/animation/Player_Fall.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_DASH, engine->getAnimation("media/animation/Player_Dash.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_PUSH, engine->getAnimation("media/animation/Player_Push.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_DAMAGE, engine->getAnimation("media/animation/Player_Damage.dae", animation.animationEntity));
    animation.animationEntity->insertAnimation(PlayerAnimation::PLAYER_ON_AIR, engine->getAnimation("media/animation/Player_On_Air.dae", animation.animationEntity));

    animation.animationEntity->playAnimation(PlayerAnimation::PLAYER_IDLE);

    // Particle Genertator to Sprint
    Transform transformGen{};
    transformGen.setPosition(0, -1, 0);
    auto *nodeGen = engine->createParticleGenerator(30, {0.3, 0.3}, nullptr, render.node, transformGen, 3);
    auto *particleGen = nodeGen->template getEntity<ParticleGenerator>();
    particleGen->addEffect(std::make_unique<DirtParticle>());
    particleGen->addColor(glm::vec4{0.6f, 0.4f, 0.2f, 0.5f});
    particleGen->addParticleLife(0.5f);
    particleGen->setGenerationRatio(2.f);
    particleGen->IsLooping = true;
    particleGen->IsRunning = false;

    render.renderNodeTypes.emplace(RenderComponent::RenderType::PARTICLE_1, nodeGen->getID());

    entityManager->template addComponent<CinematicComponent>(entity);

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);

    audio.audioList.emplace("Effects/jump", audioEngine->loadSoundEvent("Effects/jump"));
    audio.audioList.emplace("Effects/steps", audioEngine->loadSoundEvent("Effects/steps"));
    audio.audioList.emplace("Effects/dash", audioEngine->loadSoundEvent("Effects/dash"));
    audio.audioList.emplace("Effects/double_jump", audioEngine->loadSoundEvent("Effects/double_jump"));
    audio.audioList.emplace("Effects/box_move", audioEngine->loadSoundEvent("Effects/box_move"));
    audio.audioList.emplace("Effects/bow_out", audioEngine->loadSoundEvent("Effects/bow_out"));
    audio.audioList.emplace("Effects/jump_fall", audioEngine->loadSoundEvent("Effects/jump_fall"));
    audio.audioList.emplace("Effects/low_health", audioEngine->loadSoundEvent("Effects/low_health"));
    audio.audioList.emplace("Effects/potion_heal", audioEngine->loadSoundEvent("Effects/potion_heal"));
    audio.audioList.emplace("Voices/player_damage", audioEngine->loadSoundEvent("Voices/player_damage"));
    audio.audioList.emplace("Effects/obtain_object", audioEngine->loadSoundEvent("Effects/obtain_object"));

    entityManager->template addComponent<PowerUpComponent>(entity);

    // CreateSeeAndFollow(glm::vec3(37, 17, 0));
    // CreateSeeAndFollow(glm::vec3(35, 17, 0));
    // CreateSeeAndFollow(glm::vec3(33, 17, 0));
    // CreateSeeAndFollow(glm::vec3(31, 17, 0));
    // CreateSeeAndFollow(glm::vec3(29, 17, 0));
    // glm::vec3 pos{-100, 30, -230};
    // glm::vec3 recollectBalls{-105, 30, -234};
    // CreatePrueba(pos, recollectBalls);
    // CreateHearAndFly(glm::vec3(35, 17, 0));
    // CreateHearAndFly(glm::vec3(36.5, 17, 0));
    // std::cout << "aqui llego \n";

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createCamera(ECS::Entity *e_focused) const
{
    auto &entity = entityManager->createEntity();
    // entity.position = glm::vec3{0,0,10};
    auto &camera = entityManager->template addComponent<CameraComponent>(entity);
    camera.cam_behaviour = std::make_unique<ThirdPersonBehaviour>();
    entity.type = EntityType::CAMERA;

    camera.distance = 10;
    if (e_focused)
    {

        camera.cam_behaviour = std::make_unique<ThirdPersonBehaviour>();

        auto *render = e_focused->getComponent<RenderComponent>();
        render->camID = entity.getEntityKey();

        // Create Camera and add node id to know child Camera
        camera.cam = engine->createCamera(render->node);

        render->renderNodeTypes.emplace(RenderComponent::RenderType::CAMERA, camera.cam->getID());

        camera.e_key = render->getEntityKey();
        camera.distance = 5;

        camera.localFocus = {0, 0.5f, 0};

        auto *rayFocus = e_focused->template getComponent<RaycastComponent>();
        rayFocus->rays.emplace_back(glm::vec3{camera.localFocus}, glm::vec3{1, 0, 0}, 5.0f, FunctionID::RAYCAMERA);

        auto *front = e_focused->getComponent<FrontBoxComponent>();
        auto *boundingFrontBox = engine->createBoundingBox(camera.cam, Transform(), glm::vec3(0.5f, 1, 1));
        front->collider = std::make_unique<OBB>(boundingFrontBox);
    }
    else
    {
        camera.cam = engine->createCamera(nullptr);
        camera.cam_behaviour = std::make_unique<FreeCameraBehaviour>();
    }
    camera.cam->setFlags(TNode::TransformFlags::TRANSLATE);

    entityManager->template addComponent<AnimationComponent>(entity);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createBoxPuzzle(glm::vec3 position) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::BOX;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Elements/caja.obj");
    render.node->localTransform.setPosition(position);

    // PHYSICS
    entityManager->template addComponent<PhysicsComponent>(entity);
    // phy.movableByPhysics = false;

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.5f);

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;

    entityManager->template addComponent<SensoryComponent>(entity);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createPlatform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 direction, float velocity, float maxRange, bool loop, std::string meshPath) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::PLATFORM;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel(meshPath.c_str());
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);
    render.node->localTransform.setScale(scale);

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);

    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &phy = entityManager->template addComponent<PhysicsComponent>(entity);
    phy.gravityBool = false;
    phy.movableByPhysics = false;

    auto &anim = entityManager->template addComponent<AnimationComponent>(entity);
    auto *animationManager = entityManager->template getSingletonComponent<AnimationManager>();
    if (animationManager)
        animationManager->addAnimation(anim, 0, TranslateAnimation{render.node, entity.getEntityKey(), glm::normalize(direction), velocity, maxRange, loop});
    anim.animating = true;

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);
    audio.audioList.emplace("Effects/platform", audioEngine->loadSoundEvent("Effects/platform"));
    audio.soundsToUpdate.emplace_back("Effects/platform", true, nullptr);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createDoor(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::PLATFORM;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Elements/door.obj");
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);
    render.node->localTransform.setScale(scale);

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &phy = entityManager->template addComponent<PhysicsComponent>(entity);
    phy.gravityBool = false;
    phy.movableByPhysics = false;

    entityManager->template addComponent<AnimationComponent>(entity);

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);
    audio.audioList.emplace("Effects/big_door_close", audioEngine->loadSoundEvent("Effects/big_door_close"));
    audio.audioList.emplace("Effects/big_door_open", audioEngine->loadSoundEvent("Effects/big_door_open"));
    audio.audioList.emplace("Music/finish_puzle", audioEngine->loadSoundEvent("Music/finish_puzle"));

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createPlate(glm::vec3 position, glm::vec3 rotation) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::PLATE;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createModel("media/models/Elements/placa.obj");
    // TODO: Si hacemos esto el raycast no funciona correctamente, mejorar
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    // PUZZLE
    entityManager->template addComponent<PuzzleComponent>(entity);

    entityManager->template addComponent<AnimationComponent>(entity);

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);

    audio.audioList.emplace("Effects/plate_up", audioEngine->loadSoundEvent("Effects/plate_up"));
    audio.audioList.emplace("Effects/plate_down", audioEngine->loadSoundEvent("Effects/plate_down"));

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createFloorNPC(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension, float friction, float bounciness, bool defaultTexture) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::NPCFLOOR;
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);

    auto *boundingBoxNode = engine->createBoundingBox(dimension);
    boundingBoxNode->localTransform.setPosition(position);
    boundingBoxNode->localTransform.setRotate(rotation);

    body.collider = std::make_unique<OBB>(boundingBoxNode);

    body.friction = friction;
    body.bounciness = bounciness;

    return entity;
}

/* BatChing */
template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createFloor(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension, float friction, float bounciness, bool defaultTexture) const
{
    auto &entity = entityManager->createEntity();

    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);

    if (defaultTexture)
    {
        auto &render = entityManager->template addComponent<RenderComponent>(entity);
        render.node = engine->createCube(dimension);
        render.node->localTransform.setPosition(position);
        render.node->localTransform.setRotate(rotation);
        engine->setTexture("media/textures/Environment/rock_tex.png", render.node);
        auto *boundingBoxNode = engine->createBoundingBox(render.node, Transform(), dimension);
        body.collider = std::make_unique<OBB>(boundingBoxNode);

        if (friction < 0.15f)
        {
            engine->setTexture("media/textures/Environment/ice_cube_tex.png", render.node);
        }
    }
    else
    {
        auto *boundingBoxNode = engine->createBoundingBox(dimension);
        boundingBoxNode->localTransform.setPosition(position);
        boundingBoxNode->localTransform.setRotate(rotation);

        body.collider = std::make_unique<OBB>(boundingBoxNode);
    }

    body.friction = friction;
    body.bounciness = bounciness;

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createVoid(glm::vec3 position, glm::vec3 dimension) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::FALL_VOID;

    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(dimension);
    // TODO: Cambiar por AABB
    boundingBoxNode->localTransform.setPosition(position);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createArrow(glm::vec3 position) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::CONSUMABLE;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createCube({1});
    render.node->localTransform.setPosition(position);

    engine->setTexture("src/media/textures/wall.png", render.node);

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(glm::vec3(1, 1, 1), render.node);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &object = entityManager->template addComponent<ObjectComponent>(entity);
    object.type = ObjectType::ARROW;

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createDivineObject(glm::vec3 position, glm::vec3 scale, std::string pathObject) const
{
    ECS::Entity *divineEntity{nullptr};
    auto itr = divineObjectPath.find(pathObject);
    if (itr != divineObjectPath.end())
    {
        divineEntity = &entityManager->createEntity();
        auto &entity = *divineEntity;
        entity.type = EntityType::DIVINE_OBJECT;
        // RENDER
        auto &render = entityManager->template addComponent<RenderComponent>(entity);

        render.node = engine->createModel(itr->second.second.c_str());

        render.node->localTransform.setPosition(position);
        render.node->localTransform.setScale(scale);

        auto &object = entityManager->template addComponent<ObjectComponent>(entity);
        object.type = itr->second.first;

        auto &phy = entityManager->template addComponent<PhysicsComponent>(entity);
        phy.gravityBool = false;
        phy.movableByPhysics = false;

        // BODY
        auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
        auto *boundingBoxNode = engine->createBoundingBox(render.node, Transform{}, glm::vec3(3.0f));
        body.collider = std::make_unique<OBB>(boundingBoxNode);

        auto &anim = entityManager->template addComponent<AnimationComponent>(entity);
        auto *animationManager = entityManager->template getSingletonComponent<AnimationManager>();
        if (animationManager)
            animationManager->addAnimation(anim, 0, TranslateAnimation{render.node, entity.getEntityKey(), {0, 1, 0}, 20.0f, 2.0f, true});
        anim.animating = true;
    }

    return *divineEntity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createProjectile(glm::vec3 position, glm::vec3 direction, float inputForce, bool hasGravity, bool hasFriction) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::PROJECTILE;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createModel("media/models/Elements/flecha.obj");
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(render.node->direction, glm::normalize(direction));
    render.node->localTransform.setScale({1.1, 1.1, 1.1});

    // BODY
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &physics = entityManager->template addComponent<PhysicsComponent>(entity);
    physics.hasAirFriction = hasFriction;
    physics.gravityBool = hasGravity;
    physics.externForces = glm::normalize(direction) * inputForce;

    auto &autoDestroy = entityManager->template addComponent<AutoDestroyComponent>(entity);
    autoDestroy.initPoint = position;
    autoDestroy.range = 100.0f;
    autoDestroy.Time4Destruction = 4.0f;

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);
    audio.audioList.emplace("Effects/arrow_fly", audioEngine->loadSoundEvent("Effects/arrow_fly"));
    audio.audioList.emplace("Effects/arrow_hit_solid", audioEngine->loadSoundEvent("Effects/arrow_hit_solid"));
    audio.soundsToUpdate.emplace_back("Effects/arrow_fly", true, nullptr);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createCheckPoint(glm::vec3 position, glm::vec3 dimension) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::CHECKPOINT;

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(dimension);
    boundingBoxNode->localTransform.setPosition(position);
    body.collider = std::make_unique<AABB>(boundingBoxNode);

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createWallSpikes(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 direction, float velocity, float range) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::SPIKES;

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createModel("media/models/Elements/pinchos.obj");
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);
    render.node->localTransform.setScale(scale);

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &anim = entityManager->template addComponent<AnimationComponent>(entity);
    auto *animationManager = entityManager->template getSingletonComponent<AnimationManager>();
    if (animationManager)
        animationManager->addAnimation(anim, 0, TranslateAnimation{render.node, entity.getEntityKey(), glm::vec3{glm::normalize(direction)}, velocity, range, true});
    anim.animating = true;

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createChest(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string chestContain) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::CHEST;

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createModel("media/models/Elements/ChestClosed.obj", defaultShader);
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);
    render.node->localTransform.setScale(scale);

    auto *openedChest = engine->createModel("media/models/Elements/ChestOpen.obj", defaultShader, render.node);
    openedChest->IsActived = false;

    auto *boundingBoxNode = engine->createBoundingBox(render.node, Transform(), {1, 1, 1});
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);
    // AUDIO

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);
    audio.audioList.emplace("Effects/open_chest", audioEngine->loadSoundEvent("Effects/open_chest"));

    auto &object = entityManager->template addComponent<ObjectComponent>(entity);
    auto itr = powerUpsTypes.find(chestContain);
    if (itr != powerUpsTypes.end())
        object.type = itr->second;

    return entity;
}

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createClimbingWall(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::CLIMBING_WALL;

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.node = engine->createCube(dimension);
    render.node->localTransform.setPosition(position);
    render.node->localTransform.setRotate(rotation);

    // BODY
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(render.node, Transform{}, dimension);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    return entity;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::createBillboard4Interaction(const char *path, glm::vec3 position, glm::vec2 size, glm::vec3 locked_axis, EntityKey EntityAsociated) const
{

    if (EntityAsociated.id != -1)
    {
        auto *entity = entityManager->getEntityByKey(EntityAsociated);

        if (entity)
        {
            auto *frontBox = entity->template getComponent<FrontBoxComponent>();
            if (frontBox)
            {
                frontBox->currentBoard = engine->createBillboard(path, position, size, locked_axis);
                position.y += 0.5f;
                frontBox->currentBoard->addTranslateAnimation(position, 2.0f, true);
            }
        }
    }
}

template <typename GameCTX, typename Engine>
TBillboard *GameObjectFactory<GameCTX, Engine>::createBillboard4Tutorial(glm::vec3 position, glm::vec3 dimension, glm::vec3 positionBoard, glm::vec2 dimensionBoard, std::string pathBoard) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::SHOW_TUTORIAL_BOARD;

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    render.board = engine->createBillboard(pathBoard.c_str(), positionBoard, dimensionBoard, glm::vec3(0.0f));
    render.board->IsActived = false;

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(dimension);
    boundingBoxNode->localTransform.setPosition(position);
    body.collider = std::make_unique<AABB>(boundingBoxNode);

    return render.board;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::addSnowEffect(TNode *camera)
{
    Transform transformGen{};
    transformGen.setPosition(0, 0, 0);
    auto *nodeGenSnow = engine->createParticleGenerator(30, {0.1, 0.1}, nullptr, camera, transformGen, 5);

    auto *particleGenSnow = nodeGenSnow->template getEntity<ParticleGenerator>();
    particleGenSnow->addEffect(std::make_unique<SnowParticle>(camera));
    particleGenSnow->addColor(glm::vec4{1.f, 1.f, 1.f, 0.5f});
    particleGenSnow->addParticleLife(0.7f);
    particleGenSnow->setGenerationRatio(25.f);
    particleGenSnow->IsLooping = true;
    particleGenSnow->IsRunning = true;
}

template <typename GameCTX, typename Engine>
std::string GameObjectFactory<GameCTX, Engine>::getCurrentBackground(std::string levelName)
{
    auto itr = backgroundLevel.find(levelName);
    if (itr != backgroundLevel.end())
        return itr->second;
    return "media/textures/Menu/BackgroundLevels/foto8.png";
}

// #################################################
// LEVEL LOAD

template <typename GameCTX, typename Engine>
ECS::Entity &GameObjectFactory<GameCTX, Engine>::createEventChangeLevel(std::string LevelLoad, glm::vec3 position, glm::vec3 dimension) const
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::LOAD_LEVEL;

    // BODY
    auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(dimension);
    boundingBoxNode->setGlobalPosition(position);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &event = entityManager->template addComponent<EventComponent>(entity);
    event.eventName = LevelLoad;

    return entity;
}

// TODO: Mejorar el carga mapa la estructura
template <typename GameCTX, typename Engine>
int GameObjectFactory<GameCTX, Engine>::initLevel(const char *pathJson, EventManager &eventManager)
{
    // SaveGame??

    entityManager->cleanAll();
    puzzleManager->cleanAll();
    elementsLoaded = 0;

    entityManager->template addSingletonComponent<Blackboard>();

    // Init Player

    auto &player = createPlayer();

    auto &camEnt = createCamera(&player);

    // Init Entity Scene
    auto &entity = entityManager->createEntity();
    SceneKey = entity.getEntityKey();

    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // TODO: En load resources tiene que ir esto

    auto &audio = entityManager->template addComponent<AudioComponent>(entity);

    if (levelManager.isMember(pathJson))
    {
        currentLevelName = pathJson;

        auto const &currentLevelJson = levelManager[pathJson];

        // READ JSON
        std::ifstream FileLevelInfo(currentLevelJson["LevelPath"].asString());
        if (!FileLevelInfo)
            throw std::runtime_error("No se puede abrir el fichero JSON\n");

        Json::Reader reader;
        reader.parse(FileLevelInfo, currentLevelData);
        jsonItr = currentLevelData.begin();
        FileLevelInfo.close();

        // READ OBJ AND SHADERS
        auto const &meshesList = currentLevelJson["Meshes"];
        for (auto const &mesh : meshesList)
        {

            if (mesh["HasInstances"].asBool())
            {
                // Leer el json de instancias y cargar los modelados
                std::ifstream FileInstances(mesh["mesh"].asString());
                if (!FileInstances)
                    throw std::runtime_error("No se puede abrir el fichero JSON\n");

                Json::Value InstancesElements;
                reader.parse(FileInstances, InstancesElements);
                FileInstances.close();

                for (auto const &instanceObject : InstancesElements)
                {

                    std::string InstancePath = "media/models/instances/" + instanceObject["Name"].asString() + ".obj";

                    std::vector<glm::mat4> matricesTransforms4Instances{};

                    for (auto const &matrixJson : instanceObject["Matrix"])
                    {
                        glm::mat4 matrix{};

                        for (int row = 0; row < int(matrixJson.size()); row++)
                        {
                            auto const &currentRowMatJson = matrixJson[row];
                            for (int col = 0; col < 4; col++)
                            {
                                // Para cada columna, inserto el elemento de la fila
                                matrix[col][row] = currentRowMatJson[col].asFloat();
                            }
                        }

                        matricesTransforms4Instances.emplace_back(matrix);
                    }

                    if (mesh.isMember("Vegetation"))
                    {
                        glm::vec3 WindValues = JSONHelper::getVector3(mesh["Vegetation"]);
                        engine->setWind({WindValues.x, WindValues.y}, WindValues.z, true);
                        render.node = engine->addInstances4Scene(InstancePath.c_str(), matricesTransforms4Instances, nullptr, SG_VEGETATION);
                    }
                    else
                    {
                        render.node = engine->addInstances4Scene(InstancePath.c_str(), matricesTransforms4Instances, nullptr);
                    }
                }
            }
            else
            {
                render.node = engine->addModel4Scene(mesh["mesh"].asString().c_str(),
                                                     engine->getShader(mesh["shader"][0].asString().c_str(), mesh["shader"][1].asString().c_str(), mesh["IsLightShader"].asBool()));
            }
        }

        if (currentLevelJson.isMember("Cinematic"))
        {
            curentCinematicFile = currentLevelJson["Cinematic"].asString();
        }

        if (currentLevelJson.isMember("ParticleEnv"))
        {
            if (currentLevelJson["ParticleEnv"].asString() == "Snow")
            {
                auto *camera = camEnt.template getComponent<CameraComponent>();
                addSnowEffect(camera->cam);
            }
        }

        if (currentLevelJson.isMember("MusicPath") && not currentLevelJson["MusicPath"].asString().empty())
        {
            audio.audioList.emplace(currentLevelJson["MusicPath"].asString(), audioEngine->loadSoundEvent(currentLevelJson["MusicPath"].asString().c_str()));
            audio.soundsToUpdate.emplace_back(currentLevelJson["MusicPath"].asString(), true, nullptr);
        }

        if (currentLevelJson.isMember("MapPath"))
        {
            GeneratePathfinding(currentLevelJson["MapPath"].asString());
        }

        if (currentLevelJson.isMember("AmbientPath") && not currentLevelJson["AmbientPath"].asString().empty())
        {
            audio.audioList.emplace(currentLevelJson["AmbientPath"].asString(), audioEngine->loadSoundEvent(currentLevelJson["AmbientPath"].asString().c_str()));
            audio.soundsToUpdate.emplace_back(currentLevelJson["AmbientPath"].asString(), true, nullptr);
        }

        if (currentLevelJson.isMember("OnCave"))
        {
            eventManager.EventQueueInstance.EnqueueLateEvent(CaveData{true, currentLevelJson["OnCave"].asFloat()});
        }

        if (currentLevelJson.isMember("SunLight"))
        {
            LightParams params{};
            params.direction = JSONHelper::getVector3(currentLevelJson["SunLight"]["Direction"]);
            params.ambient = JSONHelper::getVector3(currentLevelJson["SunLight"]["Ambient"]);
            params.diffuse = JSONHelper::getVector3(currentLevelJson["SunLight"]["Diffuse"]);
            engine->createLight(nullptr, Transform{}, params);
        }
        if (currentLevelJson.isMember("SkyBox"))
        {
            skybox_path.clear();
            for (auto const &pathFace : currentLevelJson["SkyBox"])
                skybox_path.emplace_back(pathFace.asString());
            engine->setSkyBox(engine->getCurrentCamera().first, skybox_path);
        }
    }

    entityManager->template addComponent<SceneBodyComponent>(entity);
    return int(currentLevelData.size());
}

template <typename GameCTX, typename Engine>
int GameObjectFactory<GameCTX, Engine>::loadResourcesxFragments(GameCTX &g)
{
    // simular barra progreso cargando poco a poco
    // Range of 0-5 elements
    NumElementsToLoad = std::size_t(rand() % 5);
    entityManager = &g;

    for (std::size_t i = 0; i < NumElementsToLoad && jsonItr != currentLevelData.end(); i++, jsonItr++)
    {
        auto const &element = *jsonItr;
        auto itr = FactoryFunction.find(element["Coleccion"].asString());
        if (itr != FactoryFunction.end())
            itr->second(element);
        else
            std::cout << "\n***Error, No existe la carga del elemento: " + element["Coleccion"].asString() + " ***\n";
    }
    elementsLoaded += int(NumElementsToLoad);

    return elementsLoaded;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::loadTiles()
{
    for (auto &tile : LevelTiles)
    {
        engine->addTile({tile.x, tile.y}, {tile.z, tile.w});
    }
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::loadCinematicData()
{
    if (curentCinematicFile == "")
        return;

    std::ifstream FileCinematicInfo(curentCinematicFile);
    if (!FileCinematicInfo)
        throw std::runtime_error("No se puede abrir el fichero JSON\n");

    Json::Reader reader;
    Json::Value CinematicData;
    reader.parse(FileCinematicInfo, CinematicData);
    FileCinematicInfo.close();

    auto *playerEntity = entityManager->getEntityByKey(PlayerKey);
    int CinematicIndex{0};

    for (auto const &cinematicElements : CinematicData)
    {
        auto itr = cinematicsEvents.find(CinematicIndex);
        if (itr != cinematicsEvents.end())
        {
            auto *entity = entityManager->getEntityByKey(itr->second);
            if (entity)
            {
                auto *event = entity->template getComponent<EventComponent>();
                event->ID = itr->first;
                event->time = cinematicElements["Duration"].asFloat();
            }
        }

        for (auto const &entity : cinematicElements["Entities"])
        {

            CinematicComponent *cineComp{nullptr};
            if (entity["Entity"].asString() == "Player")
            {
                cineComp = playerEntity->template getComponent<CinematicComponent>();
            }
            else if (entity["Entity"].asString() == "Camera")
            {
                auto &ecam = createCamera();
                auto *cameraCMP = ecam.template getComponent<CameraComponent>();
                engine->setSkyBox(cameraCMP->cam, skybox_path);

                cineComp = &entityManager->template addComponent<CinematicComponent>(ecam);

                auto *camComp = ecam.template getComponent<CameraComponent>();
                camComp->globalFocus = JSONHelper::getVector3(entity["Aim"]);
            }
            else if (entity["Entity"].asString() == "HudElement")
            {
                auto &ehud = entityManager->createEntity();
                entityManager->template addComponent<RenderMenuComponent>(ehud);
                cineComp = &entityManager->template addComponent<CinematicComponent>(ehud);
            }
            else if (entity["Entity"].asString() == "SoundElement")
            {
                auto &esound = entityManager->createEntity();
                cineComp = &entityManager->template addComponent<CinematicComponent>(esound);
            }
            cineComp->initSpawnCinematic.emplace(CinematicIndex, JSONHelper::getVector3(entity["Position"]));

            CinematicComponent::ActionList actionsData{};
            for (auto const &action : entity["Actions"])
            {
                std::string stringKey;
                if (action.getMemberNames()[0] != "Duration" && action.getMemberNames()[0] != "InitTime")
                {
                    stringKey = action.getMemberNames()[0];
                }
                else if (action.getMemberNames()[1] != "Duration" && action.getMemberNames()[1] != "InitTime")
                {
                    stringKey = action.getMemberNames()[1];
                }
                else
                {
                    stringKey = action.getMemberNames()[2];
                }

                auto itr = ActionsMap.find(stringKey);

                if (itr != ActionsMap.end())
                {
                    actionsData.push_back(itr->second(action));
                    actionsData.back()->setTimes(action["InitTime"].asFloat(), action["Duration"].asFloat());
                }
                else
                    continue;
            }
            cineComp->cinematicMap.emplace(CinematicIndex, std::move(actionsData));
        }

        CinematicIndex++;
    }
    curentCinematicFile = "";
}
template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::clean()
{
    LevelTiles.clear();
    cinematicsEvents.clear();
    currentLevelData.clear();
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::removeEntity(EntityKey key)
{
    entityManager->markEntityToRemove(key);
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::setPlatformLevel(Json::Value const &element, std::string meshPath)
{
    std::string name = element["Name"].asString();
    auto position = JSONHelper::getVector3(element["Position"]);
    auto rotation = JSONHelper::getVector3(element["Rotation"]);
    auto scale = JSONHelper::getVector3(element["Scale"]);
    if (element["Children"][0])
    {
        auto child = element["Children"][0];

        auto childPos = JSONHelper::getVector3(child["Position"]);

        // calculo distancia recorre plataforma
        float d0 = (childPos[0] - position[0]);
        float d1 = (childPos[1] - position[1]);
        float d2 = (childPos[2] - position[2]);
        float distance = std::sqrt(d0 * d0 + d1 * d1 + d2 * d2);

        glm::vec3 direction = glm::normalize(glm::vec3(d0, d1, d2));

        float velocity = child["Velocity"].asFloat();

        createPlatform(position, rotation, scale, direction, velocity, distance, true, meshPath);
    }
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::GeneratePathfinding(std::string mapPath)
{
    auto *blackboard = entityManager->template getSingletonComponent<Blackboard>();

        std::ifstream file(mapPath);

        if (!file)
        {

            throw std::runtime_error("No se puede abrir el fichero JSON\n");
        }

        std::string linea;

        std::getline(file, linea);
        std::getline(file, linea);
        std::getline(file, linea);

        int x = std::stoi(linea);

        std::getline(file, linea);

        int z = std::stoi(linea);

        blackboard->matriz.resize(x, std::vector<int>(z, 0));

        std::getline(file, linea);
        std::getline(file, linea);

        int fila{0}, columna{0};

        while (std::getline(file, linea))
        {
            fila = 0;
            size_t pos = linea.find("-");
            while (pos != std::string::npos)
            {

                std::string num = linea.substr(0, pos);

                blackboard->matriz[fila][columna] = std::stoi(num);

                linea = linea.substr(pos + 1);
                pos = linea.find("-");
                fila++;
            }

            columna++;
            std::getline(file, linea);
        }
        createFloorNPC(glm::vec3(-139, 20, -463), glm::vec3(0, 0, 0), glm::vec3(200, 2.6, 200));
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::CreateProjectile)
    {
        ProjectileEventData *data{dynamic_cast<ProjectileEventData *>(eventData)};
        if (data)
            createProjectile(data->position, data->direction, data->inputForce, data->hasGravity, data->hasFriction);
        else
            std::cout << "\n***** ERROR: CREAR PROYECTIL *****\n";
    }
    else if (eventData->EventId == EventIds::ShowControlBoard)
    {
        ShowBoardData *data{dynamic_cast<ShowBoardData *>(eventData)};
        if (data)
            createBillboard4Interaction(data->path.c_str(), data->position, data->size, data->locked_axis, data->EntityAsociated);
        else
            std::cout << "\n***** ERROR: CREAR BILLBOARD *****\n";
    }
    else if (eventData->EventId == EventIds::ShowAchievement)
    {
        auto *hudEngine = engine->getHUDEngine();
        if (hudEngine)
        {
            auto *element = hudEngine->addRect({0.90f, 0.05f}, {0.208, 0.09}, {0.1, 0.1, 0.1, 1}, 5);
            element->setDestructionIn(7.0f);

            auto *data = dynamic_cast<AchievementEventData *>(eventData);
            if (data)
            {
                std::string achievementText{"Achievement: " + data->achievementID};
                auto *text = hudEngine->addText(achievementText.c_str(), glm::vec3(1.0), {0.92f, 0.045f}, 16, 6);
                text->setDestructionIn(7.0f);
                if (not data->iconPath.empty())
                {
                    auto iconPath = data->iconPath;
                    auto *icon = hudEngine->addImage(std::move(iconPath), {0.82f, 0.05f}, {0.0313, 0.055}, 6);
                    icon->setDestructionIn(7.0f);
                }
            }
        }
    }
    else if (eventData->EventId == EventIds::ThrowBall)
    {
        ThrowBallData *data{dynamic_cast<ThrowBallData *>(eventData)};
        if (data)
            ThrowBall(data->position, data->direction, data->force, data->potencia);
        else
            std::cout << "\n***** ERROR: CREAR BOLA*****\n";
    }
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::InitFactoryFunctions()
{
    FactoryFunction = {
        {"Collision", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createFloor(position, rotation, dimension);
         }},
        {"Ice", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createFloor(position, rotation, dimension, 0.1f);
         }},
        {"Rebote", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             auto bounce = element["Rebote"].asFloat();
             createFloor(position, rotation, dimension, {}, bounce);
         }},
        {"Box", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             createBoxPuzzle(position);
         }},
        {"Climb", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createClimbingWall(position, rotation, dimension);
         }},
        {"Chest", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto scale = JSONHelper::getVector3(element["Scale"]);
             std::string chestContain = element["Name"].asString();
             createChest(position, rotation, scale, chestContain);
         }},

        {"Plate", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             float friction = std::floor(element["Friction"].asFloat() * 10.0f) / 10.0f;
             if (element["Children"][0])
             {
                 Json::Value const &children = element["Children"];

                 PlatePuzzle plates_pzl{};
                 createEntitySolution solution{*this, position, rotation, dimension, friction};

                 for (auto const &child : children)
                 {
                     auto platePos = JSONHelper::getVector3(child["Position"]);
                     auto plateRot = JSONHelper::getVector3(child["Rotation"]);

                     auto &plate = createPlate(platePos, plateRot);
                     plates_pzl.addPlate(plate);
                 }

                 plates_pzl.addSolution(solution);

                 puzzleManager->addPuzzle(plates_pzl, true);
             }
         }},

        {"Platforms", [&](Json::Value const &element)
         {
             setPlatformLevel(element, "media/models/Elements/platform.obj");
         }},
        {"WallPlatforms", [&](Json::Value const &element)
         {
             setPlatformLevel(element, "media/models/Elements/wall_platform.obj");
         }},
        {"PlatePlatform", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto scale = JSONHelper::getVector3(element["Scale"]);
             if (element["Children"][0])
             {
                 Json::Value const &child = element["Children"][0];

                 auto childPos = JSONHelper::getVector3(child["Position"]);

                 float velocity = child["Velocity"].asFloat();

                 PlatePuzzle plates_pzl{};
                 createPlatformSolution solution(*this, position, childPos, scale, rotation, velocity, true);

                 for (auto const &child : element["Children"])
                 {
                     if (child != element["Children"][0])
                     {
                         auto platePos = JSONHelper::getVector3(child["Position"]);
                         auto plateRot = JSONHelper::getVector3(child["Rotation"]);

                         auto &plate = createPlate(platePos, plateRot);
                         plates_pzl.addPlate(plate);
                     }
                 }

                 plates_pzl.addSolution(solution);

                 puzzleManager->addPuzzle(plates_pzl, true);
             }
         }},
        {"PolygonWorld", [&](Json::Value const &element)
         {
             std::vector<glm::vec3> globalVertexs{};

             for (auto const &vertex : element["Vertices"])
             {
                 globalVertexs.emplace_back(JSONHelper::getVector3(vertex));
             }

             glm::vec3 globalPos{JSONHelper::getVector3(element["Position"])};

             glm::vec3 AB = globalVertexs[0] - globalVertexs[1];
             glm::vec3 AC = globalVertexs[0] - globalVertexs[2];
             glm::vec3 normal = glm::cross(AB, AC);
             if (normal.y < 0)
             {
                 normal = -(normal);
             }
             normal = glm::normalize(normal);

             float sphereRange = element["Sphere"].asFloat();

             auto *entity = entityManager->getEntityByKey(SceneKey);
             if (entity)
             {
                 auto *sceneCMP = entity->template getComponent<SceneBodyComponent>();
                 if (sceneCMP)
                     sceneCMP->world_bounds.emplace_back(globalVertexs, globalPos, normal, sphereRange);
             }
         }},
        {"PlateDoor", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto scale = JSONHelper::getVector3(element["Scale"]);
             if (element["Children"][0])
             {
                 auto const &child = element["Children"][0];
                 auto childPos = JSONHelper::getVector3(child["Position"]);

                 float d0 = (childPos[0] - position[0]);
                 float d1 = (childPos[1] - position[1]);
                 float d2 = (childPos[2] - position[2]);
                 float distance = std::sqrt(d0 * d0 + d1 * d1 + d2 * d2);

                 glm::vec3 direction = glm::normalize(glm::vec3(d0, d1, d2));

                 auto &entity = createDoor(position, rotation, scale);

                 auto *render = entity.template getComponent<RenderComponent>();

                 entity.template getComponent<AnimationComponent>();

                 PlatePuzzle plates_pzl{};
                 DoorSolution solution(render->node, entity.getEntityKey(), direction, distance);
                 for (auto const &child : element["Children"])
                 {
                     if (child != element["Children"][0])
                     {
                         // std::string name = child["Name"].asString();
                         auto platePos = JSONHelper::getVector3(child["Position"]);
                         auto plateRot = JSONHelper::getVector3(child["Rotation"]);

                         auto &plate = createPlate(platePos, plateRot);
                         plates_pzl.addPlate(plate);
                     }
                 }
                 plates_pzl.addSolution(solution);
                 puzzleManager->addPuzzle(plates_pzl, true);
             }
         }},
        {"Void", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createVoid(position, dimension);
         }},
        {"Check", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createCheckPoint(position, dimension);
         }},
        {"Spikes", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto rotation = JSONHelper::getVector3(element["Rotation"]);
             auto scale = JSONHelper::getVector3(element["Scale"]);
             if (element["Children"][0])
             {
                 std::string name = element["Name"].asString();

                 auto const &child = element["Children"][0];
                 auto childPos = JSONHelper::getVector3(child["Position"]);
                 glm::vec3 direction{childPos - glm::vec3(position.x, position.y, position.z)};

                 float distance = glm::length(direction);

                 float velocity = child["Velocity"].asFloat();

                 createWallSpikes(position, rotation, scale, glm::normalize(direction), velocity, distance);
             }
         }},
        {"Spawn", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);

             auto *player = entityManager->getEntityByKey(PlayerKey);
             if (player)
             {
                 auto *render = player->template getComponent<RenderComponent>();
                 if (render)
                     render->node->setGlobalPosition(position);

                 auto *checkpoint = player->template getComponent<CheckPointComponent>();
                 if (checkpoint)
                 {
                     checkpoint->spawnLevel.first = position;
                     checkpoint->checkpointSpawn = checkpoint->spawnLevel;
                 }
             }
         }},
        {"Next", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             createEventChangeLevel(element["Name"].asString(), position, dimension);
         }},
        {"Tiles", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);
             LevelTiles.emplace_back(position.x, position.z, dimension.x, dimension.z);
         }},
        {"Cinematic", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);

             auto &entity = entityManager->createEntity();
             entity.type = EntityType::LOAD_CINEMATIC;

             entityManager->template addComponent<EventComponent>(entity);

             auto &body = entityManager->template addComponent<WorldBodyComponent>(entity);
             auto *boundingBoxNode = engine->createBoundingBox(dimension);
             boundingBoxNode->setGlobalPosition(position);
             body.collider = std::make_unique<OBB>(boundingBoxNode);

             cinematicsEvents.emplace(element["Name"].asInt(), entity.getEntityKey());
         }},
        {"FinalObject", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto scale = JSONHelper::getVector3(element["Scale"]);
             std::string name = element["Name"].asString();
             createDivineObject(position, scale, name);
         }},
        {"Light", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             //  auto power = element["Power"].asFloat();
             auto color = JSONHelper::getVector3(element["Color"]);

             Transform transform{};
             transform.position = position;

             LightParams params{};
             params.type = LightType::SPOT_LIGHT;
             params.diffuse = color;
             params.ambient = glm::vec3(0.5);
             params.direction = glm::vec3(0, -1, 0);
             params.HasAttenuation = true;
             params.RangeLight = 7.5f;
             engine->createLight(nullptr, transform, params);
         }},
        {"Torch", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             //  auto power = element["Power"].asFloat();

             Transform transform{};
             transform.position = position;

             LightParams fireParams;
             fireParams.type = LightType::POINT_LIGHT;
             fireParams.ambient = glm::vec3(0.5f);
             fireParams.diffuse = glm::vec3(0.8, 0.3, 0.1);
             fireParams.constant = 1.f;
             fireParams.linear = 0.3f;
             fireParams.quadratic = 0;
             fireParams.HasAttenuation = true;

             auto *nodeLight = engine->createLight(nullptr, transform, fireParams, "media/models/Menu/fuego_torch_pared.obj");
             nodeLight->localTransform.setScale(0.08f, 0.08f, 0.08f);

             auto *light = nodeLight->template getEntity<TLight>();
             light->addEffect(FireLight{0.3f, 0.2f, 0.13f});

             auto *nodeGen = engine->createParticleGenerator(5, {0.1, 0.1}, nullptr, nodeLight, Transform());
             auto *particleGen = nodeGen->template getEntity<ParticleGenerator>();
             particleGen->addEffect(std::make_unique<FireParticle>());
             particleGen->addColor(glm::vec4{0.5f});
             particleGen->addParticleLife(2.0f);
             particleGen->IsLooping = true;
         }},
        {"TutorialBoard", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             auto dimension = JSONHelper::getVector3(element["Dimensions"]);

             auto const &boardNode = element["Children"][0];
             auto positionBoard = JSONHelper::getVector3(boardNode["Position"]);
             auto dimensionBoard3D = JSONHelper::getVector3(boardNode["Dimensions"]);
             auto dimensionBoard = glm::vec2(dimensionBoard3D.x, dimensionBoard3D.y);
             auto itr = tutorialBoard.find(element["Name"].asString());
             TBillboard *billboard{nullptr};
             if (itr != tutorialBoard.end())
             {
                 billboard = createBillboard4Tutorial(position, dimension, positionBoard, dimensionBoard, itr->second);
             }

             if (billboard)
             {
                 auto const &showNode = element["Children"][1];
                 auto positionHide = JSONHelper::getVector3(showNode["Position"]);
                 auto dimensionHide = JSONHelper::getVector3(showNode["Dimensions"]);

                 auto &entityHide = entityManager->createEntity();
                 entityHide.type = EntityType::HIDE_TUTORIAL_BOARD;

                 auto &render2 = entityManager->template addComponent<RenderComponent>(entityHide);
                 render2.board = billboard;

                 auto &bodyHide = entityManager->template addComponent<WorldBodyComponent>(entityHide);
                 auto *boundingBoxNodeHide = engine->createBoundingBox(dimensionHide);
                 boundingBoxNodeHide->localTransform.setPosition(positionHide);
                 bodyHide.collider = std::make_unique<AABB>(boundingBoxNodeHide);
             }
         }},
        {"SpawnIA", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             std::vector<glm::vec3> patroll{};

             for (auto const &child : element["Children"])
             {
                 auto childPosition = JSONHelper::getVector3(child["Position"]);
                 patroll.emplace_back(childPosition);
             }
             patroll.emplace_back(position);

             CreatePatrollAndPathfinding(position, patroll);
         }},
        {"SpawnPin", [&](Json::Value const &element)
         {
             auto position = JSONHelper::getVector3(element["Position"]);
             glm::vec3 patroll{};

             for (auto const &child : element["Children"])
             {
                 patroll = JSONHelper::getVector3(child["Position"]);
             }

             CreatePrueba(position, patroll);
         }},

    };
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::InitCinematicFunctions()
{
    ActionsMap = {
        {"Move2D", [](Json::Value const &info)
         { return std::make_unique<MoveAction2D>(info); }},
        {"CameraMove", [](Json::Value const &info)
         { return std::make_unique<MoveCamera>(info); }},
        {"MovePlayerCamera", [](Json::Value const &info)
         { return std::make_unique<MovePlayerCamera>(info); }},
        {"DrawText", [&](Json::Value const &info)
         { return std::make_unique<DrawText>(info, engine); }},
        {"SoundPlay", [&](Json::Value const &info)
         { return std::make_unique<PlaySound>(info, audioEngine); }},
        /* {[](Json::Value const &info)
         { return std::make_unique<MoveAction>(); }} */
    };
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::loadSaveData(Json::Value loadData)
{
    auto *player = entityManager->getEntityByKey(PlayerKey);
    if (player)
    {
        auto *abilityCMP = player->template getComponent<AbilityComponent>();
        for (int i = 0; i < int(abilities.size()) && i < loadData["Abilities"].asInt(); i++)
            abilityCMP->addAbility(abilities[i]());

        auto *powerCMP = player->template getComponent<PowerUpComponent>();
        for (auto &power : loadData["PowerUps"])
        {
            powerCMP->powerUps.emplace_back(powerUps[static_cast<PowerUpId>(power.asInt())]());
            auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
            if (eventManager)
                eventManager->EventQueueInstance.EnqueueLateEvent(PowerUpData{static_cast<PowerUpId>(power.asInt())});
        }
    }
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::CreatePrueba(glm::vec3 pos, glm::vec3 recollectBalls)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::NPC;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Characters/EnemigoDispara.obj");
    render.node->localTransform.setPosition(pos);

    // PHYSICS
    entityManager->template addComponent<PhysicsComponent>(entity);
    // phy.movableByPhysics = false;

    // AI
    auto &ai = entityManager->template addComponent<AIComponent>(entity);
    ai.place2HaveBalls = recollectBalls;
    aiFactory.createBirdTree(*ai.BehaviourTree.get());

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.5f);

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;

    auto &sensory = entityManager->template addComponent<SensoryComponent>(entity);
    sensory.RangeSight = 20.0f;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::CreateSeeAndFollow(glm::vec3 pos)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::NPC;


    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Characters/rata.obj");
    render.node->localTransform.setPosition(pos);

    // PHYSICS
    entityManager->template addComponent<PhysicsComponent>(entity);
    // phy.movableByPhysics = false;

    // AI
    auto &ai = entityManager->template addComponent<AIComponent>(entity);

    aiFactory.seeAndFollow(*ai.BehaviourTree.get());

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.5f);

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;

    auto &sensory = entityManager->template addComponent<SensoryComponent>(entity);
    sensory.RangeSight = 15.0f;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::CreatePatrollAndPathfinding(glm::vec3 pos, std::vector<glm::vec3> points4Patroll)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::NPC;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Characters/rata.obj");
    render.node->localTransform.setPosition(pos);

    // PHYSICS
    entityManager->template addComponent<PhysicsComponent>(entity);
    // phy.movableByPhysics = false;

    // AI
    auto &ai = entityManager->template addComponent<AIComponent>(entity);

    for (std::size_t i = 0; i < points4Patroll.size(); i++)
    {
        ai.patrollPoints.emplace_back(points4Patroll[i]);
    }

    aiFactory.patroll(*ai.BehaviourTree.get());

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.5f);

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;

    auto &sensory = entityManager->template addComponent<SensoryComponent>(entity);
    sensory.RangeSight = 15.0f;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::CreateHearAndFly(glm::vec3 pos)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::NPC;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);
    // render.node = engine->createCube(2);
    render.node = engine->createModel("media/models/Characters/Prometeo.obj");
    render.node->localTransform.setPosition(pos);

    // PHYSICS
    entityManager->template addComponent<PhysicsComponent>(entity);

    // AI
    auto &ai = entityManager->template addComponent<AIComponent>(entity);

    aiFactory.hearAndFly(*ai.BehaviourTree.get(), pos);

    // BODY
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &ray = entityManager->template addComponent<RaycastComponent>(entity);
    ray.rays.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{0, -1, 0}, 1.5f);

    auto &checkpoint = entityManager->template addComponent<CheckPointComponent>(entity);
    checkpoint.spawnLevel.first = render.node->localTransform.position;

    auto &sensory = entityManager->template addComponent<SensoryComponent>(entity);
    sensory.RangeHearing = 2.0f;
}

template <typename GameCTX, typename Engine>
void GameObjectFactory<GameCTX, Engine>::ThrowBall(glm::vec3 position, glm::vec3 direction, float force, float range)
{
    auto &entity = entityManager->createEntity();
    entity.type = EntityType::SNOW_BALL;

    // RENDER
    auto &render = entityManager->template addComponent<RenderComponent>(entity);

    float size = 2 + range;

    render.node = engine->createCube({size, size, size});
    auto initialPoint = position + glm::normalize(direction) * 3.0f;
    render.node->localTransform.setPosition(initialPoint);

    // BODY
    auto &body = entityManager->template addComponent<BodyComponent>(entity);
    auto *boundingBoxNode = engine->createBoundingBox(render.node);
    body.collider = std::make_unique<OBB>(boundingBoxNode);

    auto &physics = entityManager->template addComponent<PhysicsComponent>(entity);
    physics.hasAirFriction = false;
    physics.gravityBool = false;

    physics.externForces = glm::normalize(direction) * force;

    auto &autoDestroy = entityManager->template addComponent<AutoDestroyComponent>(entity);
    autoDestroy.initPoint = initialPoint;
    autoDestroy.range = range;
    autoDestroy.Time4Destruction = 5.0f;
}
