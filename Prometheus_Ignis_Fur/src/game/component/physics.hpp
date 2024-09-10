#pragma once

#include <ecs/component/component.hpp>
#include <chrono>

struct PhysicsComponent : public ECS::ComponentBase<PhysicsComponent>
{
    explicit PhysicsComponent(EntityKey e_id) : ECS::ComponentBase<PhysicsComponent>(e_id){};

    glm::vec3 gravity{0, -9.81f, 0};
    glm::vec3 force{};
    glm::vec3 velocity{};

    glm::vec3 ownForce{};
    glm::vec3 externForces{};

    float mass{1};
    bool gravityBool{true};
    bool movableByPhysics{true};
    bool hasAirFriction{true};



    bool isStatic{false};
    bool waiting4ChangeGrounded{false};
    double lastGrounded{std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count()};
    bool isGrounded{false};
    bool isBounce{false};
    bool hasInertia{false};
};
