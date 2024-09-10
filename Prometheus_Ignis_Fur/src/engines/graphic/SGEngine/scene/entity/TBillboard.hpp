#pragma once

#include "TEntity.hpp"

#include <engines/graphic/resource/ResourceTexture.hpp>

#include <functional>

struct SGEngine;
struct TBillboard : public TEntity
{
    friend SGEngine;

    explicit TBillboard(ResourceTexture *texture, glm::vec3 position, glm::vec2 size, glm::vec3 axisLocked, ResourceShader *billboardShader);

    ~TBillboard();

    void update() override final;

    void addTranslateAnimation(glm::vec3 position_to_go, float speedTranslate, bool interpolated = false);
    bool TranslateAnimation{false};

    void addSizeAnimation(glm::vec2 size_to_go, float speedSize, bool interpolated = false);
    bool SizeAnimation{false};

    glm::vec3 position{};
    glm::vec2 size{};
    glm::vec3 axis_locked;

    bool IsActived{true};

    void clear() { remove = true; };

private:
    bool remove{false};
    ResourceTexture *texture{nullptr};
    ResourceShader *shader{nullptr};
    unsigned int VAO, VBO, EBO;

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

    void updateAnimation();

    std::function<void()> calculateTranslateAnimation{nullptr};
    glm::vec3 position_to_go{};
    glm::vec3 lastPosition{};
    float speedTranslate{1.0f};

    std::function<void()> calculateSizeAnimation{nullptr};
    glm::vec2 lastSize{};
    glm::vec2 size_to_go{};
    float speedSize{1.0f};
};