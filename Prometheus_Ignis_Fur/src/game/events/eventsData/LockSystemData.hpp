#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

struct LockSystemData : EventData
{
    explicit LockSystemData(bool status)
        : EventData{EventIds::LockSystem}, status{status}
    {
    }

    bool status;
};