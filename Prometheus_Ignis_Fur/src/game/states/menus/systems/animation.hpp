
#pragma once

template <typename GameCTX>
struct AnimationMenuSystem
{
    explicit AnimationMenuSystem() = default;
    void update(GameCTX &g, float deltaTime);

private:
    bool Loading{false};
};