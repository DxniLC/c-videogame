#pragma once

#include <iostream>

#include <ecs/util/typealiases.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>

namespace ECS
{
    struct EntityManager;
};
struct TNode;
struct AnimationBase
{
    using id_type = unsigned int;

    virtual ~AnimationBase() = default;

    virtual bool update(ECS::EntityManager &entityManager, float dTime) = 0;
    virtual void revert(bool directionBool) = 0;

    bool remove{false};

    [[nodiscard]] constexpr unsigned int getID() const noexcept { return animationID; };

private:
    id_type nextAnimationID{0};

protected:
    EntityKey key{};
    TNode *node{nullptr};
    id_type animationID{nextAnimationID++};
};
