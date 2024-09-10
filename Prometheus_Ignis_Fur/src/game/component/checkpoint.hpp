
#pragma once

#include <ecs/component/component.hpp>

struct CheckPointComponent : public ECS::ComponentBase<CheckPointComponent>
{
    explicit CheckPointComponent(EntityKey e_id) : ECS::ComponentBase<CheckPointComponent>(e_id){};

    std::pair<glm::vec3, int8_t> checkpointSpawn{};
    std::pair<glm::vec3, int8_t> spawnLevel{};

};