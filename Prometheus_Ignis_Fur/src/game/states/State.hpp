
#pragma once
#include <stack>
#include <memory>
#include <type_traits>

struct StateBase
{
    virtual ~StateBase() = default;
    virtual void update() = 0;
    virtual bool alive() = 0;
};

struct StateManager
{
    explicit StateManager() = default;

    template <typename State, typename... Args>
    void addState(Args &&...args)
    {
        static_assert(std::is_base_of<StateBase, State>::value, "\n*** ERROR: Invalid State ***\n"); // se transforma en un struct con un bool value. Para saber que es un tipo heredado.//// Static_assert error compilacion
        m_states.push(std::make_unique<State>(std::forward<Args>(args)...));
    }

    template <typename State, typename... Args>
    void replaceState(Args &&...args)
    {
        static_assert(std::is_base_of<StateBase, State>::value, "\n*** ERROR: Invalid State ***\n"); // se transforma en un struct con un bool value. Para saber que es un tipo heredado.//// Static_assert error compilacion
        m_newState = {std::make_unique<State>(std::forward<Args>(args)...)};
    }

    void update()
    {
        if (alive())
        {
            if (m_newState && statesToErase == 0)
            {
                m_states.pop();
                m_states.push(std::move(m_newState));
                m_newState = nullptr;
            }

            auto &state{m_states.top()};

            if (state && state->alive() && statesToErase == 0)
            {
                state->update();
            }
            else
            {
                if (statesToErase > 0)
                    statesToErase--;
                m_states.pop();
            }
        }
    }

    constexpr void eraseStates(int numStates) noexcept
    {
        statesToErase = numStates;
    }

    bool alive() { return !m_states.empty(); }

private:
    std::stack<std::unique_ptr<StateBase>> m_states;
    std::unique_ptr<StateBase> m_newState{nullptr};

    int statesToErase{0};
};