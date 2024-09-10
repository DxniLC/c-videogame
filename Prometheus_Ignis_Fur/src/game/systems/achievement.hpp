#pragma once

template <typename GameCTX>
struct AchievementSystem
{
    explicit AchievementSystem() = default;
    void update(GameCTX &g);
};