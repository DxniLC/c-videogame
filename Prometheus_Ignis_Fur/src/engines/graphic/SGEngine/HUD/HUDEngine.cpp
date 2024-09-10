
#include "HUDEngine.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <engines/resource/TResourceManager.hpp>

#include <iostream>

#include <GLAD/src/glad.h>

HUDEngine::HUDEngine(int width, int height, ResourceShader *shaderImage, ResourceShader *shaderText, ResourceFont *defaultFont, TResourceManager *resourceManager)
    : m_width{width}, m_height{height},
      shaderImage{shaderImage}, shaderText{shaderText},
      defaultFont{defaultFont}, orthogonal{glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f, -1.0f, 1.0f)},
      resourceManager{resourceManager}
{
    m_width = {width};
    m_height = {height};
    shaderImage = {shaderImage};
    shaderText = {shaderText};
    defaultFont = {defaultFont};
    orthogonal = {glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f, -1.0f, 1.0f)};
    resourceManager = {resourceManager};
    hudElements.reserve(30);
}

void HUDEngine::drawUI()
{

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    for (auto itr = hudElements.begin(); itr < hudElements.end();)
    {
        auto *hud = itr->get();
        hud->updateStatus(deltaTime.count());
        if (hud->remove)
        {
            itr = hudElements.erase(itr);
        }
        else
        {
            if (hud->IsActive)
                hud->draw(orthogonal, m_width, m_height, screenScale, deltaTime.count());
            itr++;
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

HUDImage *HUDEngine::addImage(std::string &&path, glm::vec2 position_relative, glm::vec2 size_relative, int layer, bool centered)
{
    auto *image = resourceManager->getTexture(path.c_str());
    return addImage(image, position_relative, size_relative, layer, centered);
}

HUDImage *HUDEngine::addImage(ResourceTexture *texture, glm::vec2 position_relative, glm::vec2 size_relative, int layer, bool centered)
{

    if (size_relative.x > 1 || size_relative.y > 1)
    {
        size_relative.x = size_relative.x / float(m_width);
        size_relative.y = size_relative.y / float(m_height);
    }

    auto &element = hudElements.emplace_back(std::make_unique<HUDImage>(texture, position_relative, size_relative, layer, centered));
    element->shader = shaderImage;

    return dynamic_cast<HUDImage *>(element.get());
}

HUDText *HUDEngine::addText(const char *TextToRender, glm::vec3 color, glm::vec2 position_relative, int size, int layer, ResourceFont *font)
{
    if (not font)
        font = defaultFont;

    auto &element = hudElements.emplace_back(std::make_unique<HUDText>(std::string(TextToRender), color, position_relative, size, layer, font));
    element->shader = shaderText;
    return dynamic_cast<HUDText *>(element.get());
}

HUDRect *HUDEngine::addRect(glm::vec2 position_relative, glm::vec2 size, glm::vec4 color, int layer, bool centered)
{
    if (size.x > 1 || size.y > 1)
    {
        size.x = size.x / float(m_width);
        size.y = size.y / float(m_height);
    }

    auto &element = hudElements.emplace_back(std::make_unique<HUDRect>(position_relative, size, color, layer, centered));
    element->shader = shaderImage;

    return dynamic_cast<HUDRect *>(element.get());
}

ResourceFont *HUDEngine::getFont(const char *path)
{
    return resourceManager->getFont(path);
}

void HUDEngine::setDefaultFont(const char *path)
{
    defaultFont = resourceManager->getFont(path);
}

void HUDEngine::setFontOnText(HUDText *textElement, const char *fontPath)
{
    if (textElement)
    {
        auto *font = resourceManager->getFont(fontPath);
        if (font)
            textElement->setFont(font);
    }
}

void HUDEngine::deleteElement(HUDElement *element)
{
    if (element)
        element->remove = true;
}

void HUDEngine::clearAll()
{
    hudElements.clear();
}

void HUDEngine::updateWindow(int const &width, int const &height)
{
    float scale = float(width) / float(m_width);

    m_width = width;
    m_height = height;
    orthogonal = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f, -1.0f, 1.0f);

    screenScale *= scale;
}