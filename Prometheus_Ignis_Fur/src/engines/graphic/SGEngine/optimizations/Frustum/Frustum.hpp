#pragma once

#include "Plane.hpp"

struct Frustum
{
    Plane topFace{};
    Plane bottomFace{};

    Plane rightFace{};
    Plane leftFace{};

    Plane farFace{};
    Plane nearFace{};
};