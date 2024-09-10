#include "physics.hpp"
#include <iostream>

template <typename GameCTX>
void PhysicsSystem<GameCTX>::update(GameCTX &g, float dTime)
{
    for (auto &phy : g.template getComponents<PhysicsComponent>())
    {
        auto *eptr = g.getEntityByKey(phy.getEntityKey());
        gravityForce(phy);
        airFriction(phy);
        moveEntity(phy, dTime);

        // IMPORTANT TODO: Ahora mismo todos los nodos padre son el render pero no sabemos si puede cambiar
        auto *render = eptr->template getComponent<RenderComponent>();
        if (render && not phy.isStatic)
        {
            if (eptr->type == EntityType::PROJECTILE) // Oriented to direction of force
                render->node->localTransform.setRotate(glm::vec3(0, 0, 1), glm::normalize(phy.force));
            render->node->localTransform.setPosition(render->node->localTransform.position + phy.velocity);
            render->node->updateTransforms();
        }
    }
}

template <typename GameCTX>
void PhysicsSystem<GameCTX>::gravityForce(PhysicsComponent &physics)
{
    if (physics.gravityBool && not physics.isStatic)
    {
        physics.externForces += gravity * physics.mass;
    }
}

template <typename GameCTX>
void PhysicsSystem<GameCTX>::airFriction(PhysicsComponent &physics)
{
    if (!physics.isGrounded && physics.hasAirFriction)
    {
        float airForce = sqrt((physics.ownForce.x * physics.ownForce.x) + (physics.ownForce.z * physics.ownForce.z));
        if (airForce < 0.5)
        {
            physics.ownForce.x = 0;
            physics.ownForce.z = 0;
        }
        else
        {
            physics.ownForce.x *= (1 - (CFAirFriction));
            physics.ownForce.z *= (1 - (CFAirFriction));
        }
    }
}

template <typename GameCTX>
void PhysicsSystem<GameCTX>::moveEntity(PhysicsComponent &physics, float dTime)
{

    // TODO: Puede que con la caja vaya mal
    // Nos falla el rebote
    // if (physics.externForces.getY() >= (gravity * physics.mass).getY() && physics.externForces.getY() <= 0 && physics.isGrounded && not physics.isBounce)
    // {
    //     physics.externForces.getY() = 0;
    // }

    physics.force = physics.externForces + physics.ownForce;

    physics.velocity = (physics.force / physics.mass) * dTime * dTime;

    physics.hasInertia = false;
}
