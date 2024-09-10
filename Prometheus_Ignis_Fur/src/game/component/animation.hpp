#pragma once

#include <ecs/component/component.hpp>

struct AnimationComponent : public ECS::ComponentBase<AnimationComponent>
{
    explicit AnimationComponent(EntityKey e_id) : ECS::ComponentBase<AnimationComponent>(e_id){};

    bool animating{false};
    bool IsLooping{false};

    std::vector<unsigned int> animationsIDs{};

    int currentAnimation{-1};

    int inverseOrder{false};

    unsigned int animID{nextAnimID++};

private:
    inline static unsigned int nextAnimID{0};
};
