#pragma once

#include "HUDElement.hpp"

#include <glm/vec2.hpp>

#include <string>

#include <engines/graphic/resource/ResourceFont.hpp>

struct HUDRect : HUDElement
{

    explicit HUDRect(glm::vec2 position_relative, glm::vec2 size, glm::vec4 color, int layer, bool centered);

    ~HUDRect();

    void draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, float const &screenScale, float const &deltaTime) final;

    glm::vec2 size;
    glm::vec4 color;

private:

    float vertices[8] = {
        // position     // texture coords
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bottom right
        0.0f, 0.0f, // bottom left
        0.0f, 1.0f, // top left
    };

    unsigned int indices[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
};
