#pragma once

#include <game/manager/event/EventTrigger.hpp>
#include <game/component/raycast/RaycastResult.hpp>
#include <game/component/camera/camera.hpp>

#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

struct CameraRaycastEvent : public EventTrigger
{
    explicit CameraRaycastEvent(RaycastResult &rayresult) : result(rayresult) {}
    void handle(ECS::EntityManager &entityMan) override
    {
        // TODO: Hacer flags de propiedadeds que puedan tener varias entidades de distinto tipo
        if (result.entity_B->type == EntityType::CHECKPOINT)
            return;

        auto *cam = result.entity_A->getComponent<CameraComponent>();
        if (cam)
        {
            auto *camera = cam->cam->template getEntity<TCamera>();
            cam->cam->setGlobalPosition(result.intersection);

            auto *entity_focus = entityMan.getEntityByKey(cam->e_key);
            auto playerGlobalPosition = SGFunc::getMainNodeFromEntity(*entity_focus).globalPosition;

            auto cameraFront = playerGlobalPosition - result.intersection;
            camera->Front = glm::normalize(cameraFront + cam->localFocus);

            if (entity_focus)
            {
                auto *front = entity_focus->getComponent<FrontBoxComponent>();
                if (front)
                {
                    auto posFrontBox = cam->cam->getParent()->globalPosition + glm::normalize(camera->Front * glm::vec3(1, 0, 1)) * front->range;
                    front->collider->nodeBox->localTransform.setPosition(posFrontBox - front->collider->nodeBox->getParent()->globalPosition);
                }
            }
        }
    }
    RaycastResult result;
};
