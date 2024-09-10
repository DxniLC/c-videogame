#pragma once

#include <iostream>

#include <ecs/util/typealiases.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>

namespace ECS{
    struct EntityManager;
};
struct TNode;
struct AnimationBase
{
    virtual ~AnimationBase() = default;

    virtual bool update(ECS::EntityManager &entityManager, float dTime) = 0;

protected:
    EntityKey key{};
    TNode *node{nullptr};
};
