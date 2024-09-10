#pragma once

#include "CameraBehaviour.hpp"

struct FreeCameraBehaviour : CameraBehaviour
{
    explicit FreeCameraBehaviour() = default;
    void updateCamera(ECS::EntityManager &g, CameraComponent &ccmp, float const &deltaTime) override final;
};