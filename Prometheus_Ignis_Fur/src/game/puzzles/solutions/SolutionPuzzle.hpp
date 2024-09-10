#pragma once

#include <ecs/manager/entitymanager.hpp>

struct SolutionPuzzle{
    virtual ~SolutionPuzzle() = default;
    virtual void executeSolution(ECS::EntityManager &entityManager) = 0;
    virtual void revertSolution([[maybe_unused]] ECS::EntityManager &entityManager){};
};
