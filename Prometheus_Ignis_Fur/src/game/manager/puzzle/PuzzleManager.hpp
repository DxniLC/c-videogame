#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Puzzle.hpp"

struct PuzzleManager
{
    using PuzzleID = std::size_t;

    using PuzzleMap = std::unordered_map<PuzzleID, std::unique_ptr<Puzzle>>;

    using PuzzlePreccision = std::vector<PuzzleID>;

    explicit PuzzleManager() = default;

    // TODO: Mirar a ver de pasar un unique ptr
    template <typename PuzzleType>
    void addPuzzle(PuzzleType &puzzle, bool precission = false)
    {
        if (precission)
            puzzleIDPrecision.emplace_back(nextID);
        puzzleManager.emplace(nextID++, std::make_unique<PuzzleType>(std::move(puzzle)));
    }

    static void updatePuzzleID(PuzzleID id)
    {
        listPuzzleID_to_update.emplace_back(id);
    }

    void clearUpdatePuzzlesID()
    {
        listPuzzleID_to_update.clear();
    }

    constexpr std::vector<PuzzleID> &getPuzzlesToUpdate() const
    {
        return listPuzzleID_to_update;
    }

    PuzzleMap &getPuzzles()
    {
        return puzzleManager;
    }

    PuzzlePreccision &getPuzzlesPrecision()
    {
        return puzzleIDPrecision;
    }

    void cleanAll()
    {
        puzzleManager.clear();
        listPuzzleID_to_update.clear();
        puzzleIDPrecision.clear();
        nextID = 0;
    }

private:
    inline static std::vector<PuzzleID> listPuzzleID_to_update{};
    inline static PuzzleID nextID{0};
    // Guardamos posicion del vector y es mas rapido de acceder que en un mapa.
    PuzzleMap puzzleManager{};
    std::vector<PuzzleID> puzzleIDPrecision{};
};