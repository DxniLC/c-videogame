#include "raycast.hpp"

#include <ecs/component/EntityTypes.hpp>

#include <game/component/raycast/raycast.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>
#include <engines/graphic/SGEngine/model/Polygon.hpp>

#include <game/events/ChangeGround.hpp>
#include <game/events/Friction.hpp>
#include <game/events/CameraRay.hpp>
#include <game/events/Inertia.hpp>

template <typename GameCTX>
RaycastSystem<GameCTX>::RaycastSystem() : rules_manager{standardRaycast}
{

    rules_manager.addRule(EntityType::PLAYER, FunctionID::RAYCAMERA, CameraRay);
    rules_manager.addRule(EntityType::PLAYER, EntityType::PLATE, PressurePlate);
    rules_manager.addRule(EntityType::BOX, EntityType::PLATE, PressurePlate);

    triggerElements = EntityType::CHECKPOINT | EntityType::LOAD_CINEMATIC | EntityType::LOAD_LEVEL 
    | EntityType::FALL_VOID | EntityType::DIVINE_OBJECT | EntityType::SHOW_TUTORIAL_BOARD | EntityType::HIDE_TUTORIAL_BOARD;
}

// TODO: MEJORAR RAYCAST ORGANIZACION

template <typename GameCTX>
void RaycastSystem<GameCTX>::update(GameCTX &g)
{

    entity_manager = &g;
    event_manager = g.template getSingletonComponent<EventManager>();

    for (auto &raycmp : g.template getComponents<RaycastComponent>())
    {
        auto *ent_Ray = g.getEntityByKey(raycmp.getEntityKey());
        auto globalPosition = SGFunc::getMainNodeFromEntity(*ent_Ray).globalPosition;

        for (auto &ray : raycmp.rays)
        {

            CheckRaycast(ray, ent_Ray, globalPosition, ray.rayFlags);

            if (raycasted)
            {

                ray.result.entity_A = ent_Ray;
                ray.result.entity_B = entity_min;
                ray.result.distance = minDistance;
                ray.result.intersection = minIntersection;
                ray.result.direction = normal_plane;
                ray.result.functionID = ray.functionID;
                ray.result.has_intersection = true;

                rules_manager.applyRule(ray.result);
            }
            else if (ray.functionID == FunctionID::NONEFUNC)
            {
                event_manager->addTriggerEvent(ChangeGroundEvent{*ent_Ray, false});
            }
        }
    }

    // Sensory Rays
    for (auto &senscmp : g.template getComponents<SensoryComponent>())
    {
        auto *ent_Ray = g.getEntityByKey(senscmp.getEntityKey());
        auto globalPosition = SGFunc::getMainNodeFromEntity(*ent_Ray).globalPosition;

        for (auto &pair : senscmp.raysVision)
        {

            auto &entityKeySeen = pair.first;
            auto &ray = pair.second;
            CheckRaycast(ray, ent_Ray, globalPosition, ray.rayFlags);

            if (raycasted)
            {
                if (entityKeySeen == entity_min->getEntityKey())
                {
                    ray.result.entity_A = ent_Ray;
                    ray.result.entity_B = entity_min;
                    ray.result.distance = minDistance;
                    ray.result.intersection = minIntersection;
                    ray.result.direction = normal_plane;
                    ray.result.functionID = ray.functionID;
                    ray.result.has_intersection = true;
                }
            }
        }
    }
}

template <typename GameCTX>
void RaycastSystem<GameCTX>::standardRaycast(RaycastResult &ray)
{
    event_manager->addTriggerEvent(ChangeGroundEvent{*ray.entity_A, true});
    CollisionResult pts{};
    pts.entity_A = ray.entity_A;
    pts.entity_B = ray.entity_B;
    pts.normal = ray.direction;

    event_manager->addTriggerEvent(FrictionEvent{pts});

    auto *physics = ray.entity_A->getComponent<PhysicsComponent>();
    auto *physics_transported = ray.entity_B->getComponent<PhysicsComponent>();

    if (physics && physics_transported)
        event_manager->addTriggerEvent(InertiaEvent{physics, physics_transported});
}

template <typename GameCTX>
void RaycastSystem<GameCTX>::CameraRay(RaycastResult &ray)
{

    // Ha colisionado el raycast con direccion camara. Hay que decirle a la camara que se mueva
    auto *rcmp = ray.entity_A->getComponent<RenderComponent>();
    ray.entity_A = entity_manager->getEntityByKey(rcmp->camID);
    event_manager->addTriggerEvent(CameraRaycastEvent{ray});
}

template <typename GameCTX>
void RaycastSystem<GameCTX>::PressurePlate(RaycastResult &ray)
{
    auto *puzzle = ray.entity_B->getComponent<PuzzleComponent>();
    if (!puzzle->actived)
    {
        puzzle->actived = true;
    }
    standardRaycast(ray);
}

template <typename GameCTX>
void RaycastSystem<GameCTX>::CheckRaycast(Raycast &ray, ECS::Entity *ent_Ray, glm::vec3 const &globalPosition, RayFlagsType const &flags)
{
    // Throw raycast
    raycasted = false;
    minDistance = ray.range;
    entity_min = nullptr;
    minIntersection = glm::vec3{};
    normal_plane = glm::vec3{};

    if (flags & RayCollideFlags::RAY_SCENE)
    {
        for (auto &scene : entity_manager->template getComponents<SceneBodyComponent>())
        {
            if (ent_Ray->getEntityKey() != scene.getEntityKey())
            {
                auto *ent_Scene = entity_manager->getEntityByKey(scene.getEntityKey());
                RaycastCollision(ray, globalPosition, ent_Scene, scene.world_bounds);
            }
        }
    }

    if (flags & RayCollideFlags::RAY_WORLD)
    {

        for (auto &body : entity_manager->template getComponents<WorldBodyComponent>())
        {
            if (ent_Ray->getEntityKey() != body.getEntityKey())
            {
                auto *ent_Body = entity_manager->getEntityByKey(body.getEntityKey());
                if (not(ent_Body->type & triggerElements))
                    RaycastCollision(ray, globalPosition, ent_Body, body.collider.get());
            }
        }
    }
    if (flags & RayCollideFlags::RAY_BODY)
    {
        for (auto &body : entity_manager->template getComponents<BodyComponent>())
        {
            if (ent_Ray->getEntityKey() != body.getEntityKey())
            {
                auto *ent_Body = entity_manager->getEntityByKey(body.getEntityKey());
                RaycastCollision(ray, globalPosition, ent_Body, body.collider.get());
            }
        }
    }
}

template <typename GameCTX>
void RaycastSystem<GameCTX>::RaycastCollision(Raycast &ray, glm::vec3 const &globalPos, ECS::Entity *ent_Body, Collider *collider)
{
    auto *mesh = collider->nodeBox->template getEntity<TBoundingBox>();
    mesh->updateValues(collider->nodeBox->matrixTransformGlobal);

    auto r_point = ray.origin + globalPos;

    ray.result = RaycastResult();

    if (not SGFunc::checkCollisionRange(r_point, ray.range, mesh->globalPos, collider->collisionRange))
        return;

    for (unsigned int i = 0; i < mesh->faces.size(); i++)
    {
        auto p_normal = mesh->faces[i].normal;
        auto p_point = mesh->faces[i].center;

        if (SGFunc::Raycast_Polygon(ray, r_point, p_normal, p_point, mesh->globalVertexs, mesh->faces[i].index))
        {
            if (ray.result.distance < minDistance)
            {
                entity_min = ent_Body;
                minDistance = ray.result.distance;
                minIntersection = ray.result.intersection;
                normal_plane = ray.result.direction;
            }
            raycasted = true;
        }
    }
}

// TODO: Un plano no va a estar por encima de un objeto. Podemos ponerlo al final y si hay un raycast, no recorrerlo o algo

template <typename GameCTX>
void RaycastSystem<GameCTX>::RaycastCollision(Raycast &ray, glm::vec3 const &globalPos, ECS::Entity *ent_Body, std::vector<Polygon> const &pollygon_vector)
{

    auto r_point = ray.origin + globalPos;

    ray.result = RaycastResult();

    for (auto const &polygon : pollygon_vector)
    {

        if (not SGFunc::checkCollisionRange(r_point, ray.range, polygon.globalCenterPos, polygon.polygonRange))
        {
            continue;
        }
        if (SGFunc::Raycast_Polygon(ray, r_point, polygon.normal, polygon.globalCenterPos, polygon.globalVertexs))
        {
            if (ray.result.distance < minDistance)
            {
                entity_min = ent_Body;
                minDistance = ray.result.distance;
                minIntersection = ray.result.intersection;
                normal_plane = ray.result.direction;
            }
            raycasted = true;
        }
    }
}