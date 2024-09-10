
#include "HUDImage.hpp"

#include <engines/graphic/resource/ResourceShader.hpp>

#include <GLAD/src/glad.h>

#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

HUDImage::HUDImage(ResourceTexture *image, glm::vec2 position_relative, glm::vec2 size_relative, int layer, bool centered)
    : image{image}, size{size_relative}
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

HUDImage::~HUDImage()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void HUDImage::setAnimationSprite(unsigned int animationID, std::vector<ResourceTexture *> SpritesFrames, float Duration, bool Looping)
{
    animationsMap.emplace(animationID, HUDAnimationSprite{SpritesFrames, Duration, Looping});
}

void HUDImage::playAnimation(unsigned int animationID, bool inverse)
{
    signNextIndex = 1;

    if (inverse)
        signNextIndex = -1;

    if (not currentAnimation)
    {
        auto itr = animationsMap.find(animationID);
        if (itr != animationsMap.end())
        {
            currentAnimation = &(itr->second);
            if (inverse)
                currentAnimation->currentSprite = currentAnimation->AnimationSprites.size() - 1;
            else
                currentAnimation->currentSprite = 0;
        }
    }
}

void HUDImage::draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, [[maybe_unused]] float const &screenScale, float const &deltaTime)
{

    if (currentAnimation)
        updateAnimationSprite(deltaTime);

    auto finalSize = size * glm::vec2(screenX, screenY);

    this->FinalSize = finalSize;

    finalPos = position * glm::vec2(screenX, screenY);

    if (centered)
        finalPos = finalPos - (finalSize / 2.0f);

    if (image)
    {
        shader->use();

        shader->setInt("HUDTexture", 0);
        shader->setVec2("position", finalPos);
        shader->setVec2("size", finalSize);
        shader->setInt("layer", layer);
        shader->setMat4("orthogonalProjection", orthogonal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image->id);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void HUDImage::updateAnimationSprite(float const &deltaTime)
{

    currentFrames += deltaTime;

    if (currentFrames >= currentAnimation->Frames4Second)
    {
        auto &index = currentAnimation->currentSprite;

        if (index >= currentAnimation->AnimationSprites.size())
        {
            if (currentAnimation->AnimationHasLooping)
            {
                if (signNextIndex < 0)
                    index = currentAnimation->AnimationSprites.size() - 1;
                else
                    index = 0;
            }
            else
            {
                // Me quedo index ultimo
                int indexLimit = int(index) - signNextIndex;

                if (indexLimit < 0)
                    index = 0;
                else
                    index = currentAnimation->AnimationSprites.size() - 1;

                currentAnimation = nullptr;
                if (signNextIndex < 0)
                    image = nullptr;
            }
        }
        else
        {
            image = currentAnimation->AnimationSprites[index];
            if (signNextIndex < 0)
                index--;
            else
                index++;
        }

        currentFrames = 0;
    }
}
