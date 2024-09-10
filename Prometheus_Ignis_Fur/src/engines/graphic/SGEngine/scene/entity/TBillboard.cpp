
#include "TBillboard.hpp"

#include <GLAD/src/glad.h>

#include <engines/graphic/resource/ResourceShader.hpp>

#include <glm/gtc/matrix_transform.hpp>

TBillboard::TBillboard(ResourceTexture *texture, glm::vec3 position, glm::vec2 size, glm::vec3 axisLocked, ResourceShader *billboardShader) : position{position}, size{size}, axis_locked{axisLocked}, texture{texture}, shader{billboardShader}
{
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

TBillboard::~TBillboard()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void TBillboard::update()
{

    updateAnimation();

    shader->setMat4("model", glm::translate(glm::mat4(1.0f), position));
    shader->setVec2("BillboardSize", size);
    shader->setVec3("BillboardLockAxis", axis_locked);

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

void TBillboard::addSizeAnimation(glm::vec2 size_to_go, float speedSize, bool interpolated)
{
    this->size_to_go = size_to_go;
    lastSize = size;
    this->speedSize = speedSize;

    if (interpolated)
    {
        calculateSizeAnimation = [&]()
        {
            this->size.x = std::lerp(this->size.x, this->size_to_go.x, this->speedSize * deltaTime);
            this->size.y = std::lerp(this->size.y, this->size_to_go.y, this->speedSize * deltaTime);
        };
    }
    else
    {
        calculateSizeAnimation = [&]()
        {
            this->size += glm::normalize(this->size_to_go - this->size) * speedSize * deltaTime;
        };
    }

    SizeAnimation = true;
}

void TBillboard::addTranslateAnimation(glm::vec3 position_to_go, float speedTranslate, bool interpolated)
{
    this->position_to_go = position_to_go;
    lastPosition = position;
    this->speedTranslate = speedTranslate;

    if (interpolated)
    {
        calculateTranslateAnimation = [&]()
        {
            this->position.x = std::lerp(this->position.x, this->position_to_go.x, this->speedTranslate * deltaTime);
            this->position.y = std::lerp(this->position.y, this->position_to_go.y, this->speedTranslate * deltaTime);
            this->position.z = std::lerp(this->position.z, this->position_to_go.z, this->speedTranslate * deltaTime);
        };
    }
    else
    {
        calculateTranslateAnimation = [&]()
        {
            this->position += glm::normalize(this->position_to_go - this->position) * this->speedTranslate * deltaTime;
        };
    }

    TranslateAnimation = true;
}

void TBillboard::updateAnimation()
{
    if (TranslateAnimation)
    {
        if (calculateTranslateAnimation)
            calculateTranslateAnimation();

        auto vecDiff = position_to_go - position;

        vecDiff = glm::abs(vecDiff);

        float margin = 0.01f;

        // EndAnimation
        if (vecDiff.x <= margin && vecDiff.y <= margin && vecDiff.z <= margin)
        {
            position = position_to_go;
            position_to_go = lastPosition;
            lastPosition = position;
        }
    }

    if (SizeAnimation)
    {
        if (calculateSizeAnimation)
            calculateSizeAnimation();

        auto vecDiff = size_to_go - size;

        vecDiff = glm::abs(vecDiff);

        float margin = 0.1f;

        // EndAnimation
        if (vecDiff.x <= margin && vecDiff.y <= margin)
        {
            size = size_to_go;
            size_to_go = lastSize;
            lastSize = size;
        }
    }
}