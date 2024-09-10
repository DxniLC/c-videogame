
#pragma once
#include <util/BehaviourTree/node.hpp>
#include <chrono>
#include <iostream>
#include <glm/vec3.hpp>
#include <game/component/physics.hpp>
#include <glm/gtc/random.hpp>
#include <glm/glm.hpp>

struct BTActionScape_t : BTNode_t
{

    BTActionScape_t(glm::vec3 currentPosition)
    {
        float min{-0.8f}, max{0.8f};

        float randomValuex = glm::linearRand(-1.f, 1.f);

        while (randomValuex > min && randomValuex < max)
        {
            randomValuex = glm::linearRand(-1.f, 1.f);
        }

        float randomValuez = glm::linearRand(-1.f, 1.f);

        while (randomValuez > min && randomValuex < max)
        {
            randomValuez = glm::linearRand(-1.f, 1.f);
        }

        directionScape = glm::normalize(glm::vec3(randomValuex, 1.0f, randomValuez));

        initialPos = currentPosition;

    };

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {
        auto *phy = e.getComponent<PhysicsComponent>();
        auto *render = e.getComponent<RenderComponent>();

        if (render->node->globalPosition.y < 1000.0f)
        {
            if (glm::length(phy->ownForce) <= 500.0f)
            {
                phy->gravityBool = false;
                phy->ownForce += directionScape * 5.0f;
            }
            return BTNodeStatus_t::running;
        }

        entityManager.deleteEntityByKey(e.getEntityKey());

        return BTNodeStatus_t::success;
    }

private:
    glm::vec3 directionScape{};
    glm::vec3 initialPos{};
};