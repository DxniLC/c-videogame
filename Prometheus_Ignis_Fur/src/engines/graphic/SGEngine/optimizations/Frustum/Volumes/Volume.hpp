#pragma once

#include "../Frustum.hpp"

#include <engines/graphic/SGEngine/scene/TNode.hpp>

struct Volume
{
    virtual ~Volume() = default;

    // Para si hay tile activado
    virtual bool isOnFrustum(const Frustum &camFrustum) const = 0;
};
