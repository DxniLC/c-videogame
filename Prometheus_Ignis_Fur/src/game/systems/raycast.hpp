#pragma once

#include <game/manager/rules/Rules.hpp>

#include <ecs/component/EntityTypes.hpp>

namespace ECS
{
    struct Entity;
};
struct Raycast;
struct RaycastResult;
struct Collider;
struct Polygon;

template <typename, typename>
struct RulesManager;
template <typename GameCTX>
struct RaycastSystem
{
    explicit RaycastSystem();

    void update(GameCTX &g);

    void RaycastCollision(Raycast &ray, glm::vec3 const &globalPos, ECS::Entity *ent_Body, Collider *collider);
    void RaycastCollision(Raycast &ray, glm::vec3 const &globalPos, ECS::Entity *ent_Body, std::vector<Polygon> const &polygon);

private:

    void CheckRaycast(Raycast &ray, ECS::Entity *entity, glm::vec3 const &globalPosition, RayFlagsType const &flags);

    bool raycasted = false;
    float minDistance{5.0f};
    ECS::Entity *entity_min{nullptr};
    glm::vec3 minIntersection{};
    glm::vec3 normal_plane{};

    inline static GameCTX *entity_manager{nullptr};
    inline static EventManager *event_manager{nullptr};

    static void standardRaycast(RaycastResult &ray);
    RulesManager<RaycastResult, uint32_t> rules_manager;
    inline static uint32_t triggerElements{0};
    static void CameraRay(RaycastResult &ray);
    static void PressurePlate(RaycastResult &ray);
};
