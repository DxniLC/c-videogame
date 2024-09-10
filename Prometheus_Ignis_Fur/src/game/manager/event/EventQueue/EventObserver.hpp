#pragma once

#include "EventData.hpp"

struct EventObserver
{
    virtual ~EventObserver() = default;
    virtual void Process(EventData *eventData) = 0;

    const int ID{nextID};

private:
    inline static int nextID{0};
};