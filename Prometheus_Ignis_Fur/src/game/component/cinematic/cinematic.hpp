#pragma once

#include <ecs/component/component.hpp>

#include "actions/MoveAction2D.hpp"
#include "actions/MoveCamera.hpp"
#include "actions/MovePlayerCamera.hpp"
#include "actions/DrawText.hpp"
#include "actions/PlaySound.hpp"


#include <list>

struct CinematicComponent : public ECS::ComponentBase<CinematicComponent>
{
    explicit CinematicComponent(EntityKey e_id) : ECS::ComponentBase<CinematicComponent>(e_id){};

    using CinematicID = unsigned int;
    using ActionList = std::list<std::unique_ptr<CinematicAction>>;
    std::map<CinematicID, ActionList> cinematicMap{};
    std::map<CinematicID, glm::vec3> initSpawnCinematic{};
};
