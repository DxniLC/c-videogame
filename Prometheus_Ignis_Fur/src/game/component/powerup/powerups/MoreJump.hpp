#pragma once

#include "PowerUp.hpp"

#include <game/component/input/input.hpp>

struct MoreJump : PowerUp
{
    explicit MoreJump()
    {
        powerID = PowerUpId::MOREJUMP;
    }

    void use(ECS::Entity &entity) override final
    {
        auto *input = entity.getComponent<InputComponent>();
        if (input)
            input->jumpForce *= 1.5f;

        mStartTime = std::chrono::system_clock::now();
    }

    bool update(ECS::Entity &entity) override final
    {
        std::chrono::duration<float> mDeltaTime = std::chrono::system_clock::now() - mStartTime;

        if (mDeltaTime.count() >= coldTime)
        {
            auto *input = entity.getComponent<InputComponent>();
            if (input)
                input->jumpForce /= 1.5f;
            return true;
        }
        return false;
    }

private:
    std::chrono::system_clock::time_point mStartTime;
    float coldTime{15.0f};
};