
#pragma once

#include "SolutionPuzzle.hpp"

#include <game/factory/objectFactory.hpp>

struct createEntitySolution : public SolutionPuzzle
{
    explicit createEntitySolution(GameObjectFactory<ECS::EntityManager, SGEngine> &gf, glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension, float friction)
        : factory{&gf}, position{position}, rotation{rotation}, dimension{dimension}, friction{friction}
    {
    }

    void executeSolution([[maybe_unused]] ECS::EntityManager &entityManager) override final
    {
        if (not created)
        {
            e_key = factory->createFloor(position, rotation, dimension, friction, 0.0f, true).getEntityKey();
            created = true;
        }
    }

    void revertSolution([[maybe_unused]] ECS::EntityManager &entityManager) override final
    {
        if (created)
        {
            factory->removeEntity(e_key);
            created = false;
        }
    }

private:
    GameObjectFactory<ECS::EntityManager, SGEngine> *factory{nullptr};
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 dimension{};
    float friction{0.2f};
    bool created{false};
    EntityKey e_key{};
};