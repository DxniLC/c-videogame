#pragma once

#include <engines/input.hpp>

template <typename GameCTX>
struct InputMenuSystem
{
    explicit InputMenuSystem(InputReceiver *rec) : receiver{rec} {}

    void update(GameCTX &g);

private:
    InputReceiver *receiver{nullptr};
    [[nodiscard]] bool CheckIsHover(glm::vec2 position, glm::vec2 sizeElement);
    bool IsHovering{false};

    glm::vec2 mousePos{};

};
