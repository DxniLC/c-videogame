
#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include <game/manager/puzzle/PuzzleManager.hpp>

template <typename GameCTX>
struct PuzzleSystem : EventObserver
{
    explicit PuzzleSystem(EventManager &eventManager);
    void preupdate(GameCTX &g, PuzzleManager &puzzleManager);
    void update(GameCTX &g, PuzzleManager &puzzleManager);

    void Process(EventData *eventData) override final;

private:
    bool IsLocked{false};
    bool LockSystem(bool status);
};
