#pragma once

#include "Collider.hpp"

struct AABB : Collider
{

    explicit AABB(TNode *node);

    CollisionResult TestCollision(const Collider &collider) const override
    {
        return collider.TestCollision(*this);
    }
    CollisionResult TestCollision(const AABB &box) const override;

    CollisionResult TestCollision(const OBB &box) const override;
};