#pragma once

#include "Volume.hpp"

#include <glm/vec3.hpp>

struct Sphere : Volume
{
    explicit Sphere() = default;
    explicit Sphere(glm::vec3 center, float radius) : center{center}, radius{radius} {};

    bool isOnFrustum(const Frustum &camFrustum) const override final
    {
        return (collideOnPlane(camFrustum.farFace) &&
                collideOnPlane(camFrustum.nearFace) &&
                collideOnPlane(camFrustum.rightFace) &&
                collideOnPlane(camFrustum.leftFace) &&
                collideOnPlane(camFrustum.topFace) &&
                collideOnPlane(camFrustum.bottomFace));
    }

    bool collideOnPlane(const Plane &plane) const
    {
        // si la distancia es negativa, tienes de margen hasta el radio
        return plane.getSignedDistanceToPlane(center) > -radius;
    }

    glm::vec3 center{0.0f};
    float radius{2.0f};
};