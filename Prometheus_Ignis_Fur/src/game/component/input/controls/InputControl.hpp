#pragma once

#include <vector>
#include <functional>
#include <engines/input.hpp>
#include <iostream>

struct InputComponent;
namespace ECS
{
    struct Entity;
    struct EntityManager;
}
struct EventManager;
struct TNode;

struct InputControl
{

    virtual ~InputControl() = default;
    
    virtual void doActions(ECS::Entity &entity, ECS::EntityManager &entityManager) = 0;

    static void setReceiver(InputReceiver *currentReceiver)
    {
        receiver = currentReceiver;
    }

    void updateKeys() noexcept
    {
        keysPressed.clear();
        std::vector<std::pair<int, InputPressType>> keys{};
        for (auto &key : this->control)
        {
            keys.emplace_back(key.first);
        }
        keysPressed = receiver->getKeysPressed(keys);
        // std::cout << "\nUpdate Keys Size: " << keysPressed.size();
    }

    void processKeys() noexcept
    {
        // std::cout << "\nProcessKeys Size: " << keysPressed.size() << "\n";
        for (auto itrKeys = keysPressed.begin(); itrKeys < keysPressed.end();)
        {
            auto itr = control.find(*itrKeys);
            if (itr != control.end())
            {
                itr->second();

                if (itrKeys->second == ONETIME || itrKeys->second == RELEASED)
                    itrKeys = keysPressed.erase(itrKeys);
                else
                    itrKeys++;
            }
            else
                itrKeys++;
        }
    }

    [[nodiscard]] constexpr std::vector<std::pair<int, InputPressType>> const &getKeysPressed() const noexcept
    {
        return keysPressed;
    }

    std::function<void(InputComponent &a)> changeInput{nullptr};

protected:
    // Todos los input comparten receiver
    inline static InputReceiver *receiver{nullptr};

    struct custom_hash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            return h1 ^ h2;
        }
    };

    std::unordered_map<std::pair<int, InputPressType>, std::function<void()>, custom_hash> control{};

    std::vector<std::pair<int, InputPressType>> keysPressed{};
};