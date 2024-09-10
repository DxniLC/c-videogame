#pragma once

#include <ecs/component/component.hpp>
#include <game/component/body/collider/OBB.hpp>

#include <engines/graphic/SGEngine/scene/entity/TBillboard.hpp>

struct FrontBoxComponent : public ECS::ComponentBase<FrontBoxComponent>
{
    explicit FrontBoxComponent(EntityKey e_id) : ECS::ComponentBase<FrontBoxComponent>(e_id){};

    std::unique_ptr<Collider> collider{nullptr};
    float range{3};
    CollisionResult result{};

    TBillboard *currentBoard{nullptr};
};
