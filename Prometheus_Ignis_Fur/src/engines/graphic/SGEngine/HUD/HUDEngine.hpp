#pragma once

#include <memory>
#include <chrono>

#include "elements/HUDImage.hpp"
#include "elements/HUDText.hpp"
#include "elements/HUDRect.hpp"

struct ResourceShader;
struct SGEngine;
struct HUDEngine
{
    friend SGEngine;

    explicit HUDEngine(int width, int height, ResourceShader *shaderImage, ResourceShader *shaderText, ResourceFont *defaultFont, TResourceManager *resourceManager);

    HUDImage *addImage(std::string &&path, glm::vec2 position_relative, glm::vec2 size_relative, int layer = 0, bool centered = true);
    HUDImage *addImage(ResourceTexture *texture, glm::vec2 position_relative, glm::vec2 size_relative, int layer = 0, bool centered = true);
    HUDText *addText(const char *TextToRender, glm::vec3 color, glm::vec2 position_relative, int size, int layer = 0, ResourceFont *font = nullptr);
    HUDRect *addRect(glm::vec2 position_relative, glm::vec2 size, glm::vec4 color, int layer = 0, bool centered = true);

    void setDefaultFont(const char *path);

    void setFontOnText(HUDText *textElement, const char *fontPath);

    ResourceFont *getFont(const char *fontPath);

    void drawUI();

    void deleteElement(HUDElement *element);

    void clearAll();

    void updateWindow(int const &width, int const &height);

    bool IsRunning{true};

private:
    int m_width, m_height;

    ResourceShader *shaderImage{nullptr};
    ResourceShader *shaderText{nullptr};

    ResourceFont *defaultFont{nullptr};

    glm::mat4 orthogonal;

    TResourceManager *resourceManager{nullptr};

    float screenScale{1.0f};

    std::chrono::steady_clock::time_point lastTime{std::chrono::steady_clock::now()};

    std::vector<std::unique_ptr<HUDElement>> hudElements;
};