#pragma once

#include "EventIds.hpp"

struct EventData
{
    virtual ~EventData() = default;
    explicit EventData(EventIds eventId) : EventId{eventId} {}
    const EventIds EventId;
};