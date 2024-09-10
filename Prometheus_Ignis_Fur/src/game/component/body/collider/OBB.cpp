
#include "OBB.hpp"
#include "AABB.hpp"

#include <math.h>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>
#include <engines/graphic/SGEngine/model/Face.hpp>

#include <algorithm>

#include <game/component/body/collider/CollisionResult.hpp>

#include <game/CustomFunctions.hpp>

OBB::OBB(TNode *node)
{
    assert(node);
    this->nodeBox = node;

    auto *mesh = this->nodeBox->getEntity<TBoundingBox>();
    mesh->updateValues(this->nodeBox->matrixTransformGlobal);
    auto range = glm::length(mesh->globalPos - mesh->globalVertexs[0]);
    this->collisionRange = range+1;
}

CollisionResult OBB::TestCollision(const OBB &box) const
{
    // OBB CON OBB

    CollisionResult cpts_result;

    float depth = __FLT_MAX__;

    glm::vec3 normal_result{};

    auto *meshA = this->nodeBox->getEntity<TBoundingBox>();
    meshA->updateValues(this->nodeBox->matrixTransformGlobal);

    auto *meshB = box.nodeBox->getEntity<TBoundingBox>();
    meshB->updateValues(box.nodeBox->matrixTransformGlobal);

    if (not SGFunc::checkCollisionRange(meshB->globalPos, box.collisionRange, this->nodeBox->globalPosition, this->collisionRange))
    {
        return cpts_result;
    }


    for (std::size_t i = 0; i < meshA->faces.size(); i++)
    {
        float a_Min, a_Max, b_Min, b_Max;
        SGFunc::SAT_Algorithm(meshA->faces[i].normal, meshA->globalVertexs, a_Min, a_Max);
        SGFunc::SAT_Algorithm(meshA->faces[i].normal, meshB->globalVertexs, b_Min, b_Max);
        if (not SGFunc::overlapsInterval(a_Min, a_Max, b_Min, b_Max))
        {
            //  NO INTERSECTION
            return cpts_result;
        }
        else
        {
            // INTERSECTION
            float current_depth = a_Max - b_Min;
            depth = std::min(depth, current_depth);
            if (depth == current_depth)
            {
                normal_result = meshA->faces[i].normal;
            }
        }
    }

    for (std::size_t i = 0; i < meshA->faces.size(); i++)
    {
        float a_Min, a_Max, b_Min, b_Max;
        SGFunc::SAT_Algorithm(meshB->faces[i].normal, meshB->globalVertexs, a_Min, a_Max);
        SGFunc::SAT_Algorithm(meshB->faces[i].normal, meshA->globalVertexs, b_Min, b_Max);
        if (not SGFunc::overlapsInterval(a_Min, a_Max, b_Min, b_Max))
        {
            //  NO INTERSECTION
            return cpts_result;
        }
        else
        {
            // INTERSECTION
            float current_depth = b_Max - a_Min;
            depth = std::min(depth, current_depth);
            if (depth == current_depth)
            {
                normal_result = meshB->faces[i].normal;
            }
        }
    }

    // INTERSECTION
    cpts_result.hasCollision = true;
    cpts_result.depth = depth;
    cpts_result.normal = -normal_result;

    return cpts_result;
}

CollisionResult OBB::TestCollision(const AABB &box) const
{
    auto globalPosA = this->nodeBox->globalPosition;
    auto globalPosB = box.nodeBox->globalPosition;

    auto *boundingA = this->nodeBox->getEntity<TBoundingBox>();
    auto sizeA = boundingA->size;

    auto *boundingB = box.nodeBox->getEntity<TBoundingBox>();
    auto sizeB = boundingB->size;

    if (not SGFunc::checkCollisionRange(boundingB->globalPos, box.collisionRange, this->nodeBox->globalPosition, this->collisionRange))
    {
        CollisionResult cpts_result;
        return cpts_result;
    }

   
    return SGFunc::AABB_Collision(globalPosA, sizeA, globalPosB, sizeB);
}