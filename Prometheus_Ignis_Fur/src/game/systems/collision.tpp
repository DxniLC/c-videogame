
#include "collision.hpp"

#include <game/manager/rules/Rules.hpp>

#include <ecs/component/EntityTypes.hpp>

#include <game/component/body/body.hpp>
#include <game/component/body/worldbody.hpp>
#include <game/component/body/scenebody.hpp>

// Events
#include <game/events/CollisionCorrection.hpp>
#include <game/events/Bounciness.hpp>
#include <game/events/ClimbOn.hpp>
#include <game/events/GoToCheckpoint.hpp>
#include <game/events/RepellingForce.hpp>

// EventData
#include <game/events/eventsData/LoadLevelData.hpp>
#include <game/events/eventsData/InitCinematicData.hpp>
#include <game/events/eventsData/AbilityData.hpp>

template <typename GameCTX>
CollisionSystem<GameCTX>::CollisionSystem() : rules_manager{standardCollision}
{
    std::cout << "\n **** Collision ADD RULE **** \n";
    rules_manager.addRule((EntityType::PLAYER | EntityType::BOX), EntityType::PLATE, Player_Plate);
    // rules_manager.addRule(EntityType::BOX, EntityType::PLATE, Box_Plate);
    rules_manager.addRule(EntityType::PLAYER, EntityType::CONSUMABLE, Pick_Consumable);
    rules_manager.addRule(EntityType::PLAYER, EntityType::CHECKPOINT, SetCheckpoint);
    rules_manager.addRule(EntityType::PLAYER, EntityType::SPIKES, LostHP);
    rules_manager.addRule((EntityType::PLAYER | EntityType::BOX), (EntityType::FALL_VOID | EntityType::NPC), GoToCheckpoint);
    rules_manager.addRule(EntityType::PLAYER, EntityType::LOAD_LEVEL, LoadNewLevel);
    rules_manager.addRule(EntityType::PLAYER, EntityType::LOAD_CINEMATIC, LoadCinematic);
    rules_manager.addRule(EntityType::PLAYER, EntityType::DIVINE_OBJECT, GetDivineObject);
    rules_manager.addRule(EntityType::PLAYER, (EntityType::SHOW_TUTORIAL_BOARD | EntityType::HIDE_TUTORIAL_BOARD), ShowBillboard);
    rules_manager.addRule(EntityType::NPC, EntityType::NPCFLOOR, NPCFLOOR);
    rules_manager.addRule(EntityType::PLAYER, EntityType::SNOW_BALL, PushForce);

    triggerElements = EntityType::NPCFLOOR | EntityType::CHECKPOINT | EntityType::LOAD_CINEMATIC | EntityType::LOAD_LEVEL | EntityType::FALL_VOID | EntityType::DIVINE_OBJECT | EntityType::SHOW_TUTORIAL_BOARD | EntityType::HIDE_TUTORIAL_BOARD;
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::update(GameCTX &g)
{
    game_manager = &g;
    event_manager = g.template getSingletonComponent<EventManager>();

    // WorldPolygon with Entity
    for (auto &world : g.template getComponents<SceneBodyComponent>())
    {
        auto *e_world = g.getEntityByKey(world.getEntityKey());
        for (auto &ccmp : g.template getComponents<BodyComponent>())
        {
            if (not ccmp.canCollide)
                continue;

            auto *e_coll = g.getEntityByKey(ccmp.getEntityKey());

            CollisionResult deeperCollision{};

            deeperCollision.depth = -__FLT_MAX__;
            for (auto const &polygon : world.world_bounds)
            {
                CollisionResult pts = SGFunc::BB_Polygon_Collision(*ccmp.collider.get()->nodeBox, polygon, ccmp.collider.get()->collisionRange);
                if (pts.hasCollision)
                {
                    pts.entity_A = e_coll;
                    pts.entity_B = e_world;

                    if (pts.depth > deeperCollision.depth)
                        deeperCollision = pts;
                }
            }

            if (deeperCollision.hasCollision)
                rules_manager.applyRule(deeperCollision);
        }
    }

    // WorldElement with Entity
    for (auto &world : g.template getComponents<WorldBodyComponent>())
    {
        auto *e_world = g.getEntityByKey(world.getEntityKey());

        for (auto &ccmp : g.template getComponents<BodyComponent>())
        {
            if (not ccmp.canCollide)
                continue;

            auto e_coll = g.getEntityByKey(ccmp.getEntityKey());

            CollisionResult pts = ccmp.collider->TestCollision(*world.collider.get());
            if (pts.hasCollision)
            {
                pts.entity_A = e_world;
                pts.entity_B = e_coll;
                rules_manager.applyRule(pts);
            }
        }
    }

    // Entity with Entity
    auto &bodies_vector = g.template getComponents<BodyComponent>();
    for (std::size_t i = 0; i < bodies_vector.size(); i++)
    {
        auto &bodyA = bodies_vector[i];
        if (not bodyA.canCollide)
            continue;
        auto e_bodyA = g.getEntityByKey(bodyA.getEntityKey());
        for (std::size_t j = i + 1; j < bodies_vector.size(); j++)
        {
            auto &ccmp = bodies_vector[j];
            if (not ccmp.canCollide)
                continue;
            auto e_coll = g.getEntityByKey(ccmp.getEntityKey());
            if (e_bodyA->type == EntityType::NPC && e_coll->type == EntityType::NPC)
                continue;
            if (e_bodyA->type == EntityType::NPCFLOOR && e_coll->type == EntityType::PLAYER)
                continue;
            CollisionResult pts = ccmp.collider->TestCollision(*bodyA.collider.get());

            if (pts.hasCollision)
            {
                pts.entity_A = e_bodyA;
                pts.entity_B = e_coll;
                rules_manager.applyRule(pts);
            }
        }
    }

    // FBOX with CollisionBody
    for (auto &fbox : g.template getComponents<FrontBoxComponent>())
    {
        fbox.result = CollisionResult();
        auto e_fbox = g.getEntityByKey(fbox.getEntityKey());
        for (auto &ccmp : g.template getComponents<BodyComponent>())
        {
            if (fbox.getEntityKey() == ccmp.getEntityKey())
                continue;
            auto e_coll = g.getEntityByKey(ccmp.getEntityKey());

            CollisionResult pts = ccmp.collider->TestCollision(*fbox.collider.get());
            if (pts.hasCollision)
            {
                fbox.result.entity_A = e_fbox;
                fbox.result.entity_B = e_coll;
                fbox.result.normal = pts.normal;
                fbox.result.depth = pts.depth;
            }
        }
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::standardCollision(CollisionResult &pts)
{
    if (triggerElements & static_cast<uint32_t>(pts.entity_B->type) || triggerElements & static_cast<uint32_t>(pts.entity_A->type))
        return;

    auto *physics = pts.entity_A->getComponent<PhysicsComponent>();
    auto *physicsB = pts.entity_B->getComponent<PhysicsComponent>();

    if (triggerElements & static_cast<uint32_t>(pts.entity_B->type) || triggerElements & static_cast<uint32_t>(pts.entity_A->type))
        return;

    // TODO: Hay que ver como hacerlo generico. FALLA O PUEDE DAR FALLO

    // Regla para si no tiene fisicas, para el que mayor fuerza tenga de dos con fiscias y para objetos que son los que obligatoriamente corrigen

    // Quien tenga inercia, debe ser corregido

    if (not physics || (physicsB && physicsB->hasInertia && not physics->hasInertia) || (physicsB && physicsB->force.length() > physics->force.length()) || not physics->movableByPhysics)
    {
        std::swap(pts.entity_A, pts.entity_B);
        pts.normal = -pts.normal;
    }

    if (pts.entity_A->type == EntityType::PROJECTILE || pts.entity_B->type == EntityType::PROJECTILE)
        projectileStuck(pts);

    // TODO: Comprobar si este orden esta bien
    event_manager->addTriggerEvent(BouncinessEvent{pts});
    event_manager->addTriggerEvent(CollisionCorrectionEvent{pts});
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::Player_Plate(CollisionResult &pts)
{
    if (pts.entity_A->type == EntityType::PLATE)
    {
        std::swap(pts.entity_A, pts.entity_B);
        pts.normal = -pts.normal;
    }
    event_manager->addTriggerEvent(CollisionCorrectionEvent{pts});
    if (pts.normal.y != 0.0f)
        event_manager->addTriggerEvent(BouncinessEvent{pts});
    // event_manager->addTriggerEvent(BouncinessEvent<WorldCollisionComponent>{pts});
    // event_manager->addTriggerEvent(FrictionEvent{pts});
    event_manager->addTriggerEvent(ClimbOnEvent{pts});
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::Box_Plate(CollisionResult &pts)
{
    Player_Plate(pts);
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::Pick_Consumable(CollisionResult &pts)
{
    if (pts.entity_A->type == EntityType::CONSUMABLE)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }

    auto *inventory = pts.entity_A->template getComponent<InventoryComponent>();
    if (inventory)
    {
        auto *object = pts.entity_B->template getComponent<ObjectComponent>();
        inventory->addElement(object->type, object->quantity);

        auto *audio = pts.entity_A->template getComponent<AudioComponent>();
        if (audio)
            audio->soundsToUpdate.emplace_back("Effects/obtain_object", true, nullptr);

        game_manager->markEntityToRemove(pts.entity_B->getEntityKey());
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::SetCheckpoint(CollisionResult &pts)
{
    if (pts.entity_A->type == EntityType::CHECKPOINT)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }

    auto *checkPoint = pts.entity_A->template getComponent<CheckPointComponent>();
    if (checkPoint)
    {
        auto *body = pts.entity_B->template getComponent<WorldBodyComponent>();
        checkPoint->checkpointSpawn.first = body->collider->nodeBox->globalPosition;

        auto *health = pts.entity_A->template getComponent<HealthComponent>();
        if (health)
            checkPoint->checkpointSpawn.second = health->HP;
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::GoToCheckpoint(CollisionResult &pts)
{
    if (pts.entity_A->type == EntityType::FALL_VOID)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }

    if (pts.entity_A->type == EntityType::PLAYER)
    {
        auto *achieveManager = game_manager->template getSingletonComponent<AchievementManager>();
        if (achieveManager)
        {

            achieveManager->achievementData.currentDeaths++;
        }
    }

    LostHP(pts);
    event_manager->addTriggerEvent(GoToCheckpointEvent{*pts.entity_A});
}

// TODO: De momento solo quita vida al player
template <typename GameCTX>
void CollisionSystem<GameCTX>::LostHP(CollisionResult &pts)
{
    if (pts.entity_B->type == EntityType::PLAYER)
    {
        std::swap(pts.entity_A, pts.entity_B);
        pts.normal = -pts.normal;
    }

    event_manager->addTriggerEvent(CollisionCorrectionEvent{pts});

    auto *health = pts.entity_A->getComponent<HealthComponent>();
    if (health)
    {
        // Si caigo al vacio debe quitar si o si
        if (not health->damaged || pts.entity_B->type == FALL_VOID)
        {
            health->damaged = true;
            health->initInmunity = true;

            // Add Event to Repelling Force
            if (pts.entity_B->type != FALL_VOID)
            {
                event_manager->addTriggerEvent(RepellingForceEvent{pts, 500.0f});
            }
        }
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::projectileStuck(CollisionResult &pts)
{
    if (pts.entity_B->type == EntityType::PROJECTILE)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }

    auto *physics = pts.entity_A->getComponent<PhysicsComponent>();
    physics->isStatic = true;

    auto *destroy = pts.entity_A->getComponent<AutoDestroyComponent>();
    destroy->init = true;

    auto *body = pts.entity_A->getComponent<BodyComponent>();
    body->canCollide = false;

    auto *audio = pts.entity_A->getComponent<AudioComponent>();
    if (audio)
        audio->soundsToUpdate.emplace_back("Effects/arrow_hit_solid", true, nullptr);
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::LoadNewLevel(CollisionResult &pts)
{
    // LOAD NEW LEVEL
    if (pts.entity_A->type == EntityType::LOAD_LEVEL)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }

    auto *event = pts.entity_B->template getComponent<EventComponent>();
    event_manager->EventQueueInstance.EnqueueLateEvent(LoadLevelData{event->eventName});
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::LoadCinematic(CollisionResult &pts)
{
    // LOAD NEW LEVEL
    if (pts.entity_A->type == EntityType::LOAD_CINEMATIC)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }
    auto *event = pts.entity_B->template getComponent<EventComponent>();
    if (event && not event->Processed)
    {
        event->Processed = true;
        unsigned int ID = event->ID;
        event_manager->EventQueueInstance.EnqueueLateEvent(InitCinematicData{ID, event->time});
        event_manager->EventQueueInstance.EnqueueLateEvent(LockSystemData{true});
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::GetDivineObject(CollisionResult &pts)
{
    // LOAD NEW LEVEL
    if (pts.entity_A->type == EntityType::DIVINE_OBJECT)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }
    auto *object = pts.entity_B->template getComponent<ObjectComponent>();
    if (object && object->quantity != 0)
    {
        object->quantity = 0;
        game_manager->markEntityToRemove(pts.entity_B->getEntityKey());

        auto &ability = game_manager->template addComponent<AbilityComponent>(*pts.entity_A);
        if (object->type == ObjectType::CUERNO_DIVINE)
        {
            auto *lastAbility = ability.addAbility(std::make_unique<DashAbility>());
            event_manager->EventQueueInstance.EnqueueLateEvent(AbilityData{lastAbility->abilityID});
        }
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::ShowBillboard(CollisionResult &pts)
{
    // LOAD NEW LEVEL
    if (pts.entity_B->type == EntityType::PLAYER)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }
    auto *render = pts.entity_B->template getComponent<RenderComponent>();

    if (pts.entity_B->type == EntityType::HIDE_TUTORIAL_BOARD)
    {
        render->board->IsActived = false;
    }
    else if (pts.entity_B->type == EntityType::SHOW_TUTORIAL_BOARD)
    {
        render->board->IsActived = true;
    }
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::NPCFLOOR(CollisionResult &pts)
{
    auto *physics = pts.entity_A->template getComponent<PhysicsComponent>();
    auto *physicsB = pts.entity_B->template getComponent<PhysicsComponent>();

    if (not physics || (physicsB && physicsB->hasInertia && not physics->hasInertia) || (physicsB && physicsB->force.length() > physics->force.length()) || not physics->movableByPhysics)
    {
        std::swap(pts.entity_A, pts.entity_B);
        pts.normal = -pts.normal;
    }

    if (pts.entity_A->type == EntityType::PROJECTILE || pts.entity_B->type == EntityType::PROJECTILE)
        projectileStuck(pts);

    // TODO: Comprobar si este orden esta bien
    event_manager->addTriggerEvent(BouncinessEvent{pts});
    event_manager->addTriggerEvent(CollisionCorrectionEvent{pts});
}

template <typename GameCTX>
void CollisionSystem<GameCTX>::PushForce(CollisionResult &pts)
{
    if (pts.entity_B->type == EntityType::PLAYER)
    {
        std::swap(pts.entity_A, pts.entity_B);
    }
    auto *render = pts.entity_B->template getComponent<RenderComponent>();

    event_manager->addTriggerEvent(RepellingForceEvent{pts, 1000.0f});
}