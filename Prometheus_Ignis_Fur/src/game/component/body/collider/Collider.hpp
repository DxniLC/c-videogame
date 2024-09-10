#pragma once

#include "CollisionResult.hpp"

struct AABB;
struct OBB;
struct Transform;
struct TNode;
struct Collider
{
    virtual ~Collider() = default;
    virtual CollisionResult TestCollision(const Collider &collider) const = 0;
    virtual CollisionResult TestCollision(const AABB &collider) const = 0;
    virtual CollisionResult TestCollision(const OBB &collider) const = 0;

    TNode *nodeBox{nullptr};
    float collisionRange{1};
};