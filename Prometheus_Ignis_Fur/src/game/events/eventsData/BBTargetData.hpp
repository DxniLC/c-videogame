#pragma once

#include <game/manager/event/EventQueue/EventData.hpp>

#include <game/AI/Blackboard.hpp>

struct BBTargetData : EventData
{
    explicit BBTargetData(Blackboard *blackboard)
        : EventData{EventIds::BBTarget}, blackboard{blackboard}
    {
    }
    Blackboard *blackboard;
};