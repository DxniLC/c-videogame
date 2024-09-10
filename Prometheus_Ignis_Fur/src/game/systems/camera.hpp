#pragma once

template <typename GameCTX>
struct CameraSystem
{
    explicit CameraSystem() = default;
    void update(GameCTX &g, float const &deltaTime);
};