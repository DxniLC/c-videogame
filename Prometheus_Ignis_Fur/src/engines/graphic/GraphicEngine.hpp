#pragma once

#include <string_view>
#include <iostream>
// #include <util/enginealiases.hpp>

#include <vector>

#include <glm/vec3.hpp>

struct GLFWwindow;
struct TNode;
struct Transform;
struct GraphicEngine
{
    // explicit Engine(int width_, int height_);

    virtual ~GraphicEngine() = default;

    virtual void initScene() = 0;

    virtual void clearScene() = 0;

    virtual void drop() = 0;

    virtual void beginScene() = 0;
    virtual void drawAll() = 0;
    virtual void endScene() = 0;


    static void setWindow(GLFWwindow &win)
    {
        m_window = &win;
    }

    [[nodiscard]] GLFWwindow *getWindow() const noexcept
    {
        return m_window;
    }

protected:
    inline static int m_width{1280}, m_height{720};
    inline static GLFWwindow *m_window{nullptr};
};
