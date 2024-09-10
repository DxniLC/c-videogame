#pragma once

#include "EventTrigger.hpp"

#include <vector>
#include <memory>
#include <functional>

#include <util/function.hpp>

#include "EventQueue/EventQueue.hpp"

struct EventManager : ECS::SingletonComponentBase<EventManager>
{
    using VecEvents = std::vector<std::unique_ptr<EventTrigger>>;

    using MapObservers = std::unordered_map<std::string, std::vector<std::unique_ptr<FunctionBase>>>;

    explicit EventManager()
    {
        m_events.reserve(100);
    }

    template <typename EventType>
    void addTriggerEvent(EventType event)
    {
        m_events.emplace_back(std::make_unique<EventType>(event));
    }

    [[nodiscard]] constexpr auto begin() noexcept { return m_events.begin(); }
    [[nodiscard]] constexpr auto end() noexcept { return m_events.end(); }
    [[nodiscard]] constexpr auto cbegin() const noexcept { return m_events.cbegin(); }
    [[nodiscard]] constexpr auto cend() const noexcept { return m_events.cend(); }

    void cleanAll() noexcept
    {
        m_events.clear();
        EventQueueInstance.clear();
    }

    void clearEventTriggers()
    {
        m_events.clear();
    }

    EventQueue EventQueueInstance{};
    
private:
    VecEvents m_events{};
};
