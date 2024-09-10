
#include "AABB.hpp"
#include "OBB.hpp"
#include <math.h>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>

#include <game/component/body/collider/CollisionResult.hpp>

#include <game/CustomFunctions.hpp>

AABB::AABB(TNode *node)
{
    assert(node);
    this->nodeBox = node;

    auto *mesh = this->nodeBox->getEntity<TBoundingBox>();
    mesh->updateValues(this->nodeBox->matrixTransformGlobal);
    float range = glm::length(mesh->globalPos - mesh->globalVertexs[0]);
    this->collisionRange = range + 1.0f;
}

CollisionResult AABB::TestCollision(const AABB &box) const
{

    auto globalPosA = this->nodeBox->globalPosition;
    auto globalPosB = box.nodeBox->globalPosition;

    auto *boundingA = this->nodeBox->getEntity<TBoundingBox>();
    auto sizeA = boundingA->size;

    auto *boundingB = box.nodeBox->getEntity<TBoundingBox>();
    auto sizeB = boundingB->size;

    if (not SGFunc::checkCollisionRange(boundingB->globalPos, box.collisionRange, this->nodeBox->globalPosition, this->collisionRange))
    {
        CollisionResult collisionpts{};
        return collisionpts;
    }

    return SGFunc::AABB_Collision(globalPosA, sizeA, globalPosB, sizeB);
}
CollisionResult AABB::TestCollision(const OBB &box) const
{
    auto globalPosA = this->nodeBox->globalPosition;
    auto globalPosB = box.nodeBox->globalPosition;

    auto *boundingA = this->nodeBox->getEntity<TBoundingBox>();
    auto sizeA = boundingA->size;

    auto *boundingB = box.nodeBox->getEntity<TBoundingBox>();
    auto sizeB = boundingB->size;

    if (not SGFunc::checkCollisionRange(boundingB->globalPos, box.collisionRange, this->nodeBox->globalPosition, this->collisionRange))
    {
        CollisionResult collisionpts{};
        return collisionpts;
    }

    return SGFunc::AABB_Collision(globalPosA, sizeA, globalPosB, sizeB);
}