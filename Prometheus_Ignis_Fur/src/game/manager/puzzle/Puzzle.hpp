#pragma once

namespace ECS
{
    struct EntityManager;
}

#include <vector>

struct Puzzle
{
    virtual ~Puzzle() = default;
    virtual void handle(ECS::EntityManager &g) = 0;
    virtual void prehandle(ECS::EntityManager &g) = 0;
    bool active{false};

protected:
    std::size_t puzzleID{0};
    inline static std::size_t nextPuzzleID{0};
};