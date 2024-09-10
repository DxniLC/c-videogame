#pragma once

#include <util/BehaviourTree/behaviourtree.hpp>

#include <glm/vec3.hpp>

struct AIFactory
{
    explicit AIFactory() = default;

    void createBirdTree(BehaviourTree_t &BehaviourTree) const;
    void seeAndFollow(BehaviourTree_t &BehaviourTree) const;
    void patroll(BehaviourTree_t &BehaviourTree) const;
    void hearAndFly(BehaviourTree_t &BehaviourTree, glm::vec3 position) const;
};