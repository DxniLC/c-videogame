#pragma once

#include "CinematicAction.hpp"

#include <util/JSONHelper.hpp>

struct MovePlayerCamera : CinematicAction
{
    explicit MovePlayerCamera(Json::Value const &info)
    {
        positionTarget = JSONHelper::getVector2(info["MovePlayerCamera"]["CameraOrientation"]);
    }

    bool update([[maybe_unused]] float const &elapsedTime, ECS::Entity *entity, [[maybe_unused]] SGEngine *engine, [[maybe_unused]] float const &deltaTime) override final
    {

        auto *render = entity->template getComponent<RenderComponent>();

        auto *cameraNode = render->getNodeChild(RenderComponent::RenderType::CAMERA);
        if (cameraNode)
        {
            auto *Tcamera = cameraNode->getEntity<TCamera>();
            if (Tcamera)
            {
                Tcamera->Yaw = positionTarget.x;
                Tcamera->Pitch = positionTarget.y;
            }
        }

        return true;
    }

private:
    glm::vec2 positionTarget{};
};