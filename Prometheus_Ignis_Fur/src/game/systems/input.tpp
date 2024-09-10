#include "input.hpp"

#include <game/component/input/input.hpp>
#include <game/component/render.hpp>

// Event Data
#include <game/events/eventsData/LockSystemData.hpp>

template <typename GameCTX>
InputSystem<GameCTX>::InputSystem(InputReceiver *rec, EventManager &eventManager) : receiver(rec)
{
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);
}

template <typename GameCTX>
void InputSystem<GameCTX>::preupdate(GameCTX &g)
{
    for (auto &inp : g.template getComponents<InputComponent>())
    {
        inp.Interact = false;
        inp.controlType->updateKeys();
    }
}

template <typename GameCTX>
bool InputSystem<GameCTX>::update(GameCTX &g)
{
    if (IsLocked)
        return false;

    for (auto &inp : g.template getComponents<InputComponent>())
    {

        auto *entity = g.getEntityByKey(inp.getEntityKey());

        inp.controlType->doActions(*entity, g);

        // If input want change
        if (inp.controlType->changeInput)
        {
            inp.controlType->changeInput(inp);
            inp.controlType->changeInput = nullptr;
        }
    }
    return false;
}

template <typename GameCTX>
void InputSystem<GameCTX>::Process(EventData *eventData)
{

    if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
        {
            IsLocked = data->status;

            if (prevCursorFunc)
            {
                receiver->setCursorPosCallback(prevCursorFunc);
                prevCursorFunc = nullptr;
            }
            else
            {
                prevCursorFunc = receiver->setCursorPosCallback(nullptr);
            }
        }else
            std::cout << "\n***** ERROR LockSystem Input *****\n";

    }
}