#pragma once

#include <ecs/component/component.hpp>

struct PuzzleComponent : public ECS::ComponentBase<PuzzleComponent>
{
    explicit PuzzleComponent(EntityKey e_id) : ECS::ComponentBase<PuzzleComponent>(e_id){};

    bool actived{false};
    std::size_t puzzleID{0};
    
};
