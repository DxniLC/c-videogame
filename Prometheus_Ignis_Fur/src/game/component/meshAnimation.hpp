#pragma once

#include <ecs/component/component.hpp>

#include <unordered_map>

#include <game/animations/AnimationsIDs.hpp>
#include <engines/graphic/resource/ResourceAnimation.hpp>
#include <engines/graphic/SGEngine/scene/entity/TAnimatedMesh.hpp>

struct MeshAnimationComponent : public ECS::ComponentBase<MeshAnimationComponent>
{
    using AnimationID = uint8_t;

    explicit MeshAnimationComponent(EntityKey e_id) : ECS::ComponentBase<MeshAnimationComponent>(e_id){};

    TAnimatedMesh *animationEntity{nullptr};
};
