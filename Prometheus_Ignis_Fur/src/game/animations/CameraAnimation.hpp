#pragma once

#include "AnimationBase.hpp"

#include <game/component/animation.hpp>

#include <ecs/manager/entitymanager.hpp>

#include <game/component/camera/camera.hpp>

#include <glm/vec3.hpp>

struct CameraAnimation : public AnimationBase
{

    // Local Values to Camera Target On ThirdCamera (ZOOM ANIMATION)
    explicit CameraAnimation(TNode *node, ECS::Entity const &cameraEntity, float distanceTarget, glm::vec3 localFocusTarget, float speedTranslation, float speedRotation)
    {
        this->node = node;
        this->cameraKey = cameraEntity.getEntityKey();
        this->distanceTarget = distanceTarget;
        this->focusTarget = localFocusTarget;
        this->speedTranslation = speedTranslation;
        this->speedRotation = speedRotation;

        auto *camCMP = cameraEntity.getComponent<CameraComponent>();
        lastDistance = camCMP->distance;
        lastFocus = camCMP->localFocus;
    }

    explicit CameraAnimation(TNode *node, ECS::Entity &cameraEntity, glm::vec3 globalEndPoint, glm::vec3 globalFocusPoint, float speedTranslation, float speedRotation)
    {
        this->node = node;
        this->cameraKey = cameraEntity.getEntityKey();
        this->speedTranslation = speedTranslation;
        this->speedRotation = speedRotation;

        auto *camCMP = cameraEntity.getComponent<CameraComponent>();

        // Cambiamos el behaviour para que use uno más generico
        if (camCMP->cam_behaviour)
        {
            auto *freecam = dynamic_cast<FreeCameraBehaviour *>(camCMP->cam_behaviour.get());
            if (not freecam)
            {
                lastBehaviour = std::move(camCMP->cam_behaviour);
                camCMP->cam_behaviour = std::make_unique<FreeCameraBehaviour>();
                camCMP->globalFocus = node->getParent()->globalPosition + camCMP->localFocus;
            }
        }

        this->globalEndPoint = globalEndPoint;
        // this->globalEndFocus = globalEndFocus;
        this->globalEndFocus = globalFocusPoint;

        lastEndPoint = camCMP->cam->globalPosition;
        lastEndFocus = camCMP->globalFocus;

        funcion = 1;
    }

    bool update(ECS::EntityManager &g, float dTime) override
    {
        if (funcion == 0)
            return LocalAnimation(g, dTime);
        else
            return GlobalAnimation(g, dTime);
    }

    void revert(bool directionAnimation) override
    {
        if (directionAnimation)
        {
            remove = false;
        }
        else
        {
            distanceTarget = lastDistance;
            focusTarget = lastFocus;
            remove = true;
        }
    }

    float speedTranslation{1.0f};
    float speedRotation{1.0f};

    float lastDistance{};
    float distanceTarget{};

    glm::vec3 lastFocus{};
    glm::vec3 focusTarget{};

    glm::vec3 lastEndPoint{};
    glm::vec3 globalEndPoint{};
    glm::vec3 lastEndFocus{};
    glm::vec3 globalEndFocus{};

private:
    EntityKey cameraKey{};
    bool LocalAnimation(ECS::EntityManager &g, [[maybe_unused]] float dTime)
    {
        auto *entityCam = g.getEntityByKey(cameraKey);
        if (not entityCam)
            return true;

        auto *camCMP = entityCam->template getComponent<CameraComponent>();

        camCMP->distance = std::lerp(camCMP->distance, distanceTarget, speedTranslation * camCMP->cam->deltaTime);

        camCMP->localFocus.x = std::lerp(camCMP->localFocus.x, focusTarget.x, speedRotation * camCMP->cam->deltaTime);
        camCMP->localFocus.y = std::lerp(camCMP->localFocus.y, focusTarget.y, speedRotation * camCMP->cam->deltaTime);
        camCMP->localFocus.z = std::lerp(camCMP->localFocus.z, focusTarget.z, speedRotation * camCMP->cam->deltaTime);

        auto *entityFocused = g.getEntityByKey(camCMP->e_key);

        if (entityFocused)
        {
            auto *raycmp = entityFocused->template getComponent<RaycastComponent>();
            if (raycmp)
            {
                for (auto &ray : raycmp->rays)
                {
                    if (ray.functionID == FunctionID::RAYCAMERA)
                    {
                        ray.range = camCMP->distance;
                        ray.origin = camCMP->localFocus;
                    }
                }
            }
        }

        if (std::abs(camCMP->localFocus.x - focusTarget.x) <= 0.001f)
            camCMP->localFocus.x = focusTarget.x;
        if (std::abs(float(camCMP->localFocus.y - focusTarget.y)) <= 0.001f)
            camCMP->localFocus.y = focusTarget.y;
        if (std::abs(float(camCMP->localFocus.z - focusTarget.z)) <= 0.001f)
            camCMP->localFocus.z = focusTarget.z;

        if (std::abs(distanceTarget - camCMP->distance) <= 0.0001f)
            camCMP->distance = distanceTarget;

        if (camCMP->cam_behaviour)
            camCMP->cam_behaviour->need_update = true;

        if (camCMP->distance == distanceTarget && camCMP->localFocus == focusTarget && remove)
            return true;

        return false;
    }

    bool GlobalAnimation(ECS::EntityManager &g, float dTime)
    {
        auto *entityCam = g.getEntityByKey(cameraKey);
        if (not entityCam)
            return true;

        auto *camCMP = entityCam->template getComponent<CameraComponent>();

        auto currentPosition = camCMP->cam->globalPosition;

        currentPosition.x = std::lerp(currentPosition.x, globalEndPoint.x, speedTranslation * dTime);
        currentPosition.y = std::lerp(currentPosition.y, globalEndPoint.y, speedTranslation * dTime);
        currentPosition.z = std::lerp(currentPosition.z, globalEndPoint.z, speedTranslation * dTime);

        if (std::abs(float(currentPosition.x - globalEndPoint.x)) <= 0.001f)
            currentPosition.x = globalEndPoint.x;
        if (std::abs(float(currentPosition.y - globalEndPoint.y)) <= 0.001f)
            currentPosition.y = globalEndPoint.y;
        if (std::abs(float(currentPosition.z - globalEndPoint.z)) <= 0.001f)
            currentPosition.z = globalEndPoint.z;

        camCMP->cam->setGlobalPosition(currentPosition);

        camCMP->globalFocus.x = std::lerp(camCMP->globalFocus.x, globalEndFocus.x, speedRotation * dTime);
        camCMP->globalFocus.y = std::lerp(camCMP->globalFocus.y, globalEndFocus.y, speedRotation * dTime);
        camCMP->globalFocus.z = std::lerp(camCMP->globalFocus.z, globalEndFocus.z, speedRotation * dTime);

        if (std::abs(float(camCMP->globalFocus.x - globalEndFocus.x)) <= 0.001f)
            camCMP->globalFocus.x = globalEndFocus.x;
        if (std::abs(float(camCMP->globalFocus.y - globalEndFocus.y)) <= 0.001f)
            camCMP->globalFocus.y = globalEndFocus.y;
        if (std::abs(float(camCMP->globalFocus.z - globalEndFocus.z)) <= 0.001f)
            camCMP->globalFocus.z = globalEndFocus.z;

        camCMP->cam_behaviour->need_update = true;

        if (currentPosition == globalEndPoint && camCMP->globalFocus == globalEndFocus && remove)
            return true;

        return false;
    }

    int funcion{0};

    std::unique_ptr<CameraBehaviour> lastBehaviour{nullptr};
};