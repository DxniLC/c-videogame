#pragma once

#include "CinematicAction.hpp"

#include <util/JSONHelper.hpp>

struct MoveCamera : CinematicAction
{
    explicit MoveCamera(Json::Value const &info)
    {
        positionTarget = JSONHelper::getVector3(info["CameraMove"]["positionMove"]);
        speed = info["CameraMove"]["speed"].asFloat();
    }

    bool update([[maybe_unused]] float const &elapsedTime, ECS::Entity *entity, SGEngine *engine, float const &deltaTime) override final
    {

        auto *camera = entity->template getComponent<CameraComponent>();

        engine->setCameraActive(camera->cam->getID());

        glm::vec3 position{camera->cam->globalPosition};

        glm::vec3 leftDistance{positionTarget - position};

        glm::vec3 direction{glm::normalize(leftDistance)};
        // TODO delta time!!!!

        camera->cam->setGlobalPosition(position + (direction * (speed * deltaTime)));

        float currentDistance = glm::length(leftDistance);

        const float margin{0.1f};

        if (currentDistance <= margin)
        {
            camera->cam->setGlobalPosition(positionTarget);
            return true;
        }

        return false;
    }

private:
    glm::vec3 positionTarget{};
    float speed{0};
};