#pragma once

#include "../entity/light/LightParams.hpp"

struct LightEffect
{
    virtual ~LightEffect() = default;
    virtual void update(LightParams &params, float const &deltaTime) = 0;
};