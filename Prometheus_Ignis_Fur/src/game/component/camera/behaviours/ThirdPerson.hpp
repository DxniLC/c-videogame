#pragma once

#include "CameraBehaviour.hpp"

struct ThirdPersonBehaviour : CameraBehaviour
{
    explicit ThirdPersonBehaviour() { this->need_update = true; }
    void updateCamera(ECS::EntityManager &g, CameraComponent &ccmp, float const &deltaTime) override final;
};