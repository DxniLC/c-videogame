#pragma once

#include <ecs/component/component.hpp>

#include <chrono>

struct AutoDestroyComponent : public ECS::ComponentBase<AutoDestroyComponent>
{
    explicit AutoDestroyComponent(EntityKey e_id) : ECS::ComponentBase<AutoDestroyComponent>(e_id){};

    using TimePoint = std::chrono::system_clock::time_point;

    // Time 4 Destruction
    bool init{false};
    float Time4Destruction{2.0f};
    TimePoint mStartTime{};

    // Destruction Range
    glm::vec3 initPoint{};
    float range{0.0f};

};
