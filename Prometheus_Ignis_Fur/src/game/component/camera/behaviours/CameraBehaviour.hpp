#pragma once

namespace ECS
{
    struct EntityManager;
}
struct CameraComponent;
struct CameraBehaviour
{
    virtual ~CameraBehaviour() = default;
    virtual void updateCamera(ECS::EntityManager &g, CameraComponent &ccmp, float const &deltaTime) = 0;

    void setCameraToUpdate(float xoffset, float yoffset)
    {
        this->xoffset = xoffset;
        this->yoffset = yoffset;
        need_update = true;
    }

    bool need_update{false};

protected:
    float xoffset{}, yoffset{};
};