#pragma once

#include "Volume.hpp"

#include <glm/vec3.hpp>

struct AABBNode : Volume
{
    AABBNode(const glm::vec3 &center, const glm::vec3 &dimensions)
        : center{center},
          extents{dimensions / 2.0f} {}

    AABBNode(const glm::vec3 &inCenter, float x, float y, float z)
        : center{inCenter}, extents{x, y, z} {}

    bool isOnFrustum(const Frustum &camFrustum) const override final
    {
        return (isOnOrForwardPlane(camFrustum.farFace) &&
                isOnOrForwardPlane(camFrustum.nearFace) &&
                isOnOrForwardPlane(camFrustum.rightFace) &&
                isOnOrForwardPlane(camFrustum.leftFace) &&
                isOnOrForwardPlane(camFrustum.topFace) &&
                isOnOrForwardPlane(camFrustum.bottomFace));
    }

    bool isOnOrForwardPlane(const Plane &plane) const
    {
        const float r = extents.x * std::abs(plane.Normal.x) +
                        extents.y * std::abs(plane.Normal.y) + extents.z * std::abs(plane.Normal.z);

        return plane.getSignedDistanceToPlane(center) > -r;
    }

    glm::vec3 center{0.f, 0.f, 0.f};
    glm::vec3 extents{0.f, 0.f, 0.f};
    glm::vec3 forwarDirection{0.f, 0.f, 0.f};
};