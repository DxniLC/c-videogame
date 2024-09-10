
#include "HUDText.hpp"

#include <GLAD/src/glad.h>

#include <engines/graphic/resource/ResourceShader.hpp>

#include <iostream>

HUDText::HUDText(std::string text_to_render, glm::vec3 colorText, glm::vec2 position_relative, int sizeText, int layer, ResourceFont *font)
    : text{text_to_render}, colorText{colorText}, size{sizeText}, font{font}
{
    position = position_relative;
    this->layer = layer;

    finalSize = float(size) / float(font->getFontSize());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 6 vertices por cada 4 floats ya que vamos a hacer un quad
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

HUDText::~HUDText()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void HUDText::setLimits(float relative_screen_X, float betweenLines)
{
    HasLimits = true;
    relativeLimit_X = relative_screen_X;
    SpaceBtwnLines = betweenLines;
}

void HUDText::setFont(ResourceFont *font)
{
    this->font = font;
}

void HUDText::draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, float const &screenScale, float const &deltaTime)
{
    // activate corresponding render state
    shader->use();
    shader->setVec3("textColor", colorText);
    shader->setMat4("orthogonalProjection", orthogonal);
    shader->setInt("layer", layer);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // TODO: PARA EL REESCALADO
    // auto finalSize = size * glm::vec2(screenX, screenY);

    updateAnimation(deltaTime);

    float finalScaleX = finalSize * screenScale * scale.x;
    float finalScaleY = finalSize * screenScale * scale.y;

    auto finalPosition = position * glm::vec2(screenX, screenY);

    auto limitX = relativeLimit_X * float(screenX);

    // iterate through all characters

    float finalAdvance{0.0f};

    if (HasLimits)
    {
        finalPosition.x = finalPosition.x - (limitX / 2.0f);
    }
    else
    {
        for (auto &c : text)
        {
            Character ch = font->getCharacterOnMap(c);
            finalAdvance += float(ch.Advance >> 6) * finalScaleX;
        }

        finalPosition.x = finalPosition.x - (finalAdvance / 2.0f);
    }

    this->FinalSize.x = finalAdvance;
    this->FinalSize.y = float(font->getFontSize()) * finalScaleY;

    // Centramos el elemento para que sea desde el punto central su dibujado y posicion

    this->finalPos = finalPosition;
    finalPos.y -= this->FinalSize.y / 2.0f;
    finalPosition.y += this->FinalSize.y / 2.0f;


    for (auto c_itr = text.begin(); c_itr < text.end(); c_itr++)
    {
        Character ch = font->getCharacterOnMap(*c_itr);

        float xpos = finalPosition.x + float(ch.Bearing.x) * finalScaleX;
        float ypos = finalPosition.y - float(ch.Bearing.y) * finalScaleY;

        float width = float(ch.Size.x) * finalScaleX;
        float height = float(ch.Size.y) * finalScaleY;

        // update VBO for characters
        float vertices[6][4] = {
            {xpos, ypos + height, 0.0f, 1.0f},
            {xpos, ypos, 0.0f, 0.0f},
            {xpos + width, ypos, 1.0f, 0.0f},

            {xpos, ypos + height, 0.0f, 1.0f},
            {xpos + width, ypos, 1.0f, 0.0f},
            {xpos + width, ypos + height, 1.0f, 1.0f}};

        // Renderzamos el glyph sobre la textura
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Obtenemos el contenido del VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Renderizamos el Quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Avanzamos la posicion al siguiente gliph
        float nextPosition = finalPosition.x + float(ch.Advance >> 6) * finalScaleX;

        if (HasLimits && (nextPosition - finalPos.x) >= limitX)
        {
            finalPosition.y += SpaceBtwnLines;
            finalPosition.x = this->finalPos.x;
        }
        else
            finalPosition.x = nextPosition; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}