#pragma once

#include <vector>
#include "Collider.hpp"

struct OBB : Collider
{

    explicit OBB(TNode *node);

    CollisionResult TestCollision(const Collider &collider) const override
    {
        return collider.TestCollision(*this);
    }

    CollisionResult TestCollision(const AABB &box) const override;

    CollisionResult TestCollision(const OBB &box) const override;
};