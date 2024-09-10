#pragma once

#include <glm/mat4x4.hpp>

#include <iostream>

#include <functional>

enum HUDAnimationType
{
    HUD_ANIM_NONE,
    HUD_ANIM_LOOP,
    HUD_ANIM_ROUND_TRIP,
};

struct ResourceShader;
struct HUDEngine;
struct HUDElement
{

    friend HUDEngine;

    virtual ~HUDElement() = default;

    virtual void draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, float const &screenScale, float const &deltaTime) = 0;

    void addAnimationTranslate(glm::vec2 position_relative, float speed, HUDAnimationType animationType = HUD_ANIM_NONE)
    {
        position_to_go = position_relative;
        lastPosition = position;
        TranslateSpeed = speed;

        if (animationType == HUD_ANIM_LOOP)
        {
            animTransFunc = [&]()
            {
                position = position_to_go;

                position_to_go = lastPosition;
                lastPosition = position;
            };
        }
        else if (animationType == HUD_ANIM_ROUND_TRIP)
        {
            animTransFunc = [&]()
            {
                position = position_to_go;

                position_to_go = lastPosition;
                lastPosition = position;

                animTransFunc = nullptr;
            };
        }

        flags |= HUD_TRANSLATE;
    }

    void addAnimationScale(glm::vec2 scale, float speed, HUDAnimationType animationType = HUD_ANIM_NONE)
    {
        scale_to_go = scale;
        lastScale = glm::vec2(1.0f);
        ScaleSpeed = speed;

        if (animationType == HUD_ANIM_LOOP)
        {
            if (not animScaleFunc)
            {
                animScaleFunc = [&]()
                {
                    scale = scale_to_go;
                    scale_to_go = lastScale;

                    lastScale = scale;
                };
            }
        }
        else if (animationType == HUD_ANIM_ROUND_TRIP)
        {
            if (not animScaleFunc)
            {
                animScaleFunc = [&]()
                {
                    scale = scale_to_go;
                    scale_to_go = lastScale;

                    lastScale = scale;

                    animScaleFunc = nullptr;
                };
            }
        }

        flags |= HUD_SCALE;
    }

    // TODO: Cambiar la interpolacion

    void updateAnimation(float deltaTime)
    {
        if (flags & HUD_TRANSLATE)
        {
            auto vecDiff = position_to_go - position;
            position += glm::normalize(vecDiff) * TranslateSpeed * deltaTime;

            vecDiff = glm::abs(vecDiff);

            float margin = 0.005f;

            // EndAnimation
            if (vecDiff.x <= margin && vecDiff.y <= margin)
            {

                if (animTransFunc)
                    animTransFunc();
                else
                {
                    position = position_to_go;
                    flags = flags xor HUD_TRANSLATE;
                }
            }
        }

        if (flags & HUD_SCALE)
        {
            auto vecDiff = scale_to_go - scale;
            scale += glm::normalize(vecDiff) * ScaleSpeed * deltaTime;

            vecDiff = glm::abs(vecDiff);

            float margin = 0.005f;

            // EndAnimation
            if (vecDiff.x <= margin && vecDiff.y <= margin)
            {

                if (animScaleFunc)
                    animScaleFunc();
                else
                {
                    scale = scale_to_go;
                    flags = flags xor HUD_SCALE;
                }
            }
        }
    }

    void updateStatus(float deltaTime)
    {
        if(not autoDestruction)
            return;
        secondsToRemove -= deltaTime;
        if (secondsToRemove > 0.0f)
            return;
        remove = true;
    }

    void setDestructionIn(float seconds)
    {
        autoDestruction = true;
        secondsToRemove = seconds;
    }

    bool IsActive{true};

    glm::vec2 position{};
    glm::vec2 scale{1.0f};

    constexpr void clean() noexcept { remove = true; };

    constexpr glm::vec2 getSize() const noexcept { return FinalSize; };
    constexpr glm::vec2 getPosition() const noexcept { return finalPos; };

    // -10 -> 10 layers [20 layers]
    int layer{0};

private:
    bool remove{false};
    bool autoDestruction{false};
    float secondsToRemove{};

protected:
    unsigned int VBO, VAO, EBO;
    ResourceShader *shader{nullptr};

    glm::vec2 FinalSize;

    glm::vec2 finalPos;

    bool centered{true};

    int screenX, screenY;

    // ANIMATION
    using type_flags = uint8_t;

    enum HUDFlags : type_flags
    {
        HUD_TRANSLATE = 1 << 1,
        HUD_SCALE = 1 << 2,
    };

    type_flags flags{0};

    // Translate Animation Var
    glm::vec2 lastPosition;
    glm::vec2 position_to_go;
    float TranslateSpeed{3.0f};
    std::function<void()> animTransFunc{nullptr};

    // Scale Animation Var
    glm::vec2 lastScale;
    glm::vec2 scale_to_go;
    float ScaleSpeed{3.0f};
    std::function<void()> animScaleFunc{nullptr};
};