#pragma once

#include "HUDElement.hpp"

#include <glm/vec2.hpp>

#include <string>

#include <engines/graphic/resource/ResourceFont.hpp>

struct HUDText : HUDElement
{

    explicit HUDText(std::string text_to_render, glm::vec3 colorText, glm::vec2 position_relative, int sizeText, int layer, ResourceFont *font);

    ~HUDText();

    void setLimits(float relative_screen_X, float betweenLines);

    void draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, float const &screenScale, float const &deltaTime) final;

    void setFont(ResourceFont *font);

    std::string text{};

    glm::vec3 colorText{0.0f};

    int size{32};

private:
    float relativeLimit_X{0.0f};
    float SpaceBtwnLines{1.0f};
    bool HasLimits{false};

    ResourceFont *font{nullptr};
    float finalSize;
};
