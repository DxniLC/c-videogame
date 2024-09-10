
#include "HUDRect.hpp"

#include <engines/graphic/resource/ResourceShader.hpp>

#include <GLAD/src/glad.h>

#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

HUDRect::HUDRect(glm::vec2 position_relative, glm::vec2 size_relative, glm::vec4 color, int layer, bool centered)
    : size{size_relative}, color{color}
{
    position = position_relative;

    this->layer = layer;
    this->centered = centered;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position and texture Coord attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
}

HUDRect::~HUDRect()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void HUDRect::draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, [[maybe_unused]] float const &screenScale, [[maybe_unused]] float const &deltaTime)
{

    auto finalSize = size * glm::vec2(screenX, screenY);

    this->FinalSize = finalSize;

    finalPos = position * glm::vec2(screenX, screenY);

    if (centered)
        finalPos = finalPos - (finalSize / 2.0f);

    shader->use();

    shader->setVec2("position", finalPos);
    shader->setVec2("size", finalSize);
    shader->setInt("layer", layer);
    shader->setVec4("color", color);
    shader->setBool("HasRect", true);
    shader->setMat4("orthogonalProjection", orthogonal);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    shader->setBool("HasRect", false);
}