#pragma once

#include <ecs/component/entity.hpp>

struct PhysicsComponent;
template <typename GameCTX>
struct PhysicsSystem
{
    explicit PhysicsSystem() = default;

    void update(GameCTX &g, float dTime);

    void gravityForce(PhysicsComponent &ent);
    void airFriction(PhysicsComponent &ent);
    void moveEntity(PhysicsComponent &ent, float dTime);

private:
    glm::vec3 gravity{0, -9.81f * 6.f, 0};
    // Si cambia friction, se cambia inertia
    float CFAirFriction{0.035f};
};
