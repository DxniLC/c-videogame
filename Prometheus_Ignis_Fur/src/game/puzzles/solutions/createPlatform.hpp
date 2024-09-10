#pragma once

#include "SolutionPuzzle.hpp"

#include <game/factory/objectFactory.hpp>

struct createPlatformSolution : public SolutionPuzzle
{
    explicit createPlatformSolution(GameObjectFactory<ECS::EntityManager, SGEngine> &gf, glm::vec3 pos, glm::vec3 posFinal, glm::vec3 scale, glm::vec3 rot, float velocity, bool loop)
        : factory{&gf}, position{pos}, positionFinal{posFinal}, scale{scale}, rotation{rot}, velocity{velocity}, loop{loop}
    {
    }

    void executeSolution([[maybe_unused]] ECS::EntityManager &entityManager) override final
    {
        if (not created)
        {
            auto d0 = (positionFinal[0] - position[0]);
            auto d1 = (positionFinal[1] - position[1]);
            auto d2 = (positionFinal[2] - position[2]);
            auto distance = sqrt(d0 * d0 + d1 * d1 + d2 * d2);

            auto direction = glm::normalize(glm::vec3(d0, d1, d2));

            e_key = factory->createPlatform(position, rotation, scale, direction, velocity, distance, loop).getEntityKey();
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
    glm::vec3 positionFinal{};
    glm::vec3 scale{};
    glm::vec3 rotation{};
    float velocity{100.0f};
    bool loop{false};
    bool created{false};
    EntityKey e_key{};
};