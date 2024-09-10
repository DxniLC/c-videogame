#pragma once

#include "HUDElement.hpp"

#include <engines/graphic/resource/ResourceTexture.hpp>

#include <map>

struct HUDAnimationSprite
{
    HUDAnimationSprite(std::vector<ResourceTexture *> &sprites, float ChangexFrame, bool IsLoop)
        : AnimationSprites{sprites}, Frames4Second{ChangexFrame}, AnimationHasLooping{IsLoop} {}
    std::vector<ResourceTexture *> AnimationSprites;
    float Frames4Second{1.0f};
    bool AnimationHasLooping{false};
    std::size_t currentSprite{0};
};

struct HUDImage : HUDElement
{

    explicit HUDImage(ResourceTexture *image, glm::vec2 position_relative, glm::vec2 size_relative, int layer, bool centered);

    void setAnimationSprite(unsigned int animationID, std::vector<ResourceTexture *> SpritesFrames, float Frames4Second, bool Looping = false);

    void playAnimation(unsigned int animationID, bool inverse = false);

    ~HUDImage();

    void draw(glm::mat4 const &orthogonal, int const &screenX, int const &screenY, float const &screenScale, float const &deltaTime) final;

    bool IsOnAnimation() const noexcept { return currentAnimation; }

    ResourceTexture *image{nullptr};

    glm::vec2 size{};


private:
    void updateAnimationSprite(float const &deltaTime);

    std::map<unsigned int, HUDAnimationSprite> animationsMap{};

    HUDAnimationSprite *currentAnimation{nullptr};

    int signNextIndex{1};

    float currentFrames{0};

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