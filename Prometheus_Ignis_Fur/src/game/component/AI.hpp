
#pragma once

#include <ecs/component/component.hpp>

#include <util/BehaviourTree/behaviourtree.hpp>

#include <glm/vec3.hpp>
#include <vector>

struct AIComponent : public ECS::ComponentBase<AIComponent>
{
    explicit AIComponent(EntityKey e_id) : ECS::ComponentBase<AIComponent>(e_id){};

    std::unique_ptr<BehaviourTree_t> BehaviourTree{std::make_unique<BehaviourTree_t>()};
    
    // ###################################
    // AI BASE
    glm::vec3 lastSeen{};
    glm::vec3 lastHeard{};


    float rangeToAdvise{50.0f};

    // PATROLL AND PATHFINDING

    std::vector<glm::vec3> patrollPoints;
    std::vector<glm::vec3> pathResult;
    bool resetCount{false};

    // ###################################

    // ###################################
    // SEE AND FLOK

    // ###################################

    // ###################################
    // PENGUIN
    bool haveBall{false};
    bool haveIBack{true};
    float charge{0.f};
    float nextStep{0.05f};
    glm::vec2 chargeRewards{10, 2};
    glm::vec2 shootRewards{1, 4};
    glm::vec2 ranges{5, 50};
    glm::vec3 lastPlaceISee{};
    glm::vec3 place2HaveBalls{}; 
    // ###################################
};