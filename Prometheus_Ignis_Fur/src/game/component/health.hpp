#pragma once

#include <ecs/component/component.hpp>

#include <chrono>

template <typename>
struct HealthSystem;

struct HealthComponent : public ECS::ComponentBase<HealthComponent>
{

    explicit HealthComponent(EntityKey e_id) : ECS::ComponentBase<HealthComponent>(e_id){};

    bool damaged{false};

    int8_t damage{1};
    int8_t HP{1};

    float inmunityTime{1.5f};
    bool initInmunity{false};

private:
    friend HealthSystem<ECS::EntityManager>;
    std::chrono::system_clock::time_point mStartTime{};
    std::chrono::duration<float> mDeltaTime{};
};
