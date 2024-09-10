
#include "puzzle.hpp"

template <typename GameCTX>
PuzzleSystem<GameCTX>::PuzzleSystem(EventManager &eventManager)
{
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);
}

template <typename GameCTX>
void PuzzleSystem<GameCTX>::update(GameCTX &g, PuzzleManager &puzzleManager)
{
    if (IsLocked)
        return;

    for (auto &puzzleID : puzzleManager.getPuzzlesToUpdate())
    {
        puzzleManager.getPuzzles()[puzzleID]->handle(g);
    }
    puzzleManager.clearUpdatePuzzlesID();
}

template <typename GameCTX>
void PuzzleSystem<GameCTX>::preupdate(GameCTX &g, PuzzleManager &puzzleManager)
{
    if (IsLocked)
        return;

    for (auto &puzzleID : puzzleManager.getPuzzlesPrecision())
    {
        puzzleManager.getPuzzles()[puzzleID]->prehandle(g);
        puzzleManager.updatePuzzleID(puzzleID);
    }
}

template <typename GameCTX>
void PuzzleSystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
            IsLocked = data->status;
        else
            std::cout << "\n***** ERROR LockSystem Puzzle *****\n";
    }
}