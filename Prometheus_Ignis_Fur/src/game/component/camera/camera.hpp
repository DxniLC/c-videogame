#pragma once

#include <ecs/component/component.hpp>
#include <ecs/component/entity.hpp>
#include <memory>

#include <game/component/camera/behaviours/FreeCam.hpp>
#include <game/component/camera/behaviours/ThirdPerson.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>

struct CameraComponent : public ECS::ComponentBase<CameraComponent>
{
    explicit CameraComponent(EntityKey e_id) : ECS::ComponentBase<CameraComponent>(e_id){};

    void setTarget(EntityKey target)
    {
        e_key = target;
    }

    TNode *cam{nullptr};
    float distance{11};
    EntityKey e_key{};

    glm::vec3 localFocus{0.f, 0.f, 0.f};
    glm::vec3 globalFocus{0.f, 0.f, 0.f};

    std::unique_ptr<CameraBehaviour> cam_behaviour{nullptr};
};