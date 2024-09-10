
#pragma once
#include <ecs/component/component.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

struct CameraMenuComponent : public ECS::ComponentBase<CameraMenuComponent>
{

    explicit CameraMenuComponent(EntityKey e_id) : ECS::ComponentBase<CameraMenuComponent>(e_id){};

    std::pair<TNode*,TCamera*> camera{};

    glm::vec3 Position{0.0f};

    glm::vec3 Rotation{0.0f};
    float RotationSpeed{5.0f};

};
