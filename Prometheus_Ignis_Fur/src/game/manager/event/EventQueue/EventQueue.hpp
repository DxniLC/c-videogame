#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>

#include "EventObserver.hpp"

// EventQueue + Observer Pattern
struct EventQueue
{

    void Subscribe(EventIds eventId, EventObserver *eventObserver)
    {
        auto itr = _observers.find(eventId);
        if (itr != _observers.end())
            itr->second.emplace_back(eventObserver);
        else
            _observers.emplace(eventId, std::vector<EventObserver *>{eventObserver});
    }

    void Unsubscribe(EventIds eventId, EventObserver *eventObserver)
    {
        if (eventObserver)
        {
            auto itr = _observers.find(eventId);
            if (itr != _observers.end())
            {
                // TODO: cambiar para que no sea busqueda lineal
                for (auto obs_itr = itr->second.begin(); obs_itr < itr->second.end(); itr++)
                {
                    if ((*obs_itr)->ID == eventObserver->ID)
                    {
                        itr->second.erase(obs_itr);
                        return;
                    }
                }
            }
        }
        else
            std::cout << "\n**** ERROR: OBSERER NO ENCONTRADO ****\n";
    }

    template <typename EventTypeData>
    void EnqueueEvent(EventTypeData eventData)
    {
        _nextEvents.emplace_back(std::make_unique<EventTypeData>(eventData));
    }

    template <typename EventTypeData>
    void EnqueueLateEvent(EventTypeData eventData)
    {
        _lateEvents.emplace_back(std::make_unique<EventTypeData>(eventData));
    }

    void update()
    {
        ProcessEvents();
    }

    // hacer un update a final de frame
    void lateUpdate()
    {
        ProcessLateEvents();
    }

    void clear()
    {
        _currentEvents.clear();
        _nextEvents.clear();
        _lateEvents.clear();
        _observers.clear();
    }

private:
    std::vector<std::unique_ptr<EventData>> _currentEvents{};
    std::vector<std::unique_ptr<EventData>> _nextEvents{};
    std::vector<std::unique_ptr<EventData>> _lateEvents{};

    std::unordered_map<EventIds, std::vector<EventObserver *>> _observers{};

    void ProcessEvent(EventData *eventData)
    {
        auto itr = _observers.find(eventData->EventId);
        if (itr != _observers.end())
        {
            for (auto &eventObserver : itr->second)
            {
                eventObserver->Process(eventData);
            }
        }
    }

    void ProcessEvents()
    {

        std::swap(_currentEvents, _nextEvents);

        for (auto &currentEvent : _currentEvents)
            ProcessEvent(currentEvent.get());

        _currentEvents.clear();

        /*if (_nextEvents.size() > 0)
        {
            ProcessEvents();
        }*/
    }

    void ProcessLateEvents()
    {

        for (auto &currentEvent : _lateEvents)
            ProcessEvent(currentEvent.get());

        _lateEvents.clear();
    }
};