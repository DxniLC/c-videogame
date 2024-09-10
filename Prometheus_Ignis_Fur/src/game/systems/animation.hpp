#pragma once

#include <game/manager/animation/AnimationManager.hpp>

template <typename GameCTX>
struct AnimationSystem
{
    explicit AnimationSystem() = default;
    void update(GameCTX &g, float dTime);
};
