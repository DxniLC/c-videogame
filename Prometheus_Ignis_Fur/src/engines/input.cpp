
#include "input.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

InputReceiver::InputReceiver(GLFWwindow &w) : window(&w)
{
    glfwGetWindowSize(window, &width, &height);
}

InputReceiver::~InputReceiver()
{
    clearCallbacks();
}

void InputReceiver::clearCallbacks()
{
    if (cursorPosCallBack)
        glfwSetCursorPosCallback(window, nullptr);
    if (windowSizeCallBack)
        glfwSetWindowSizeCallback(window, nullptr);

    cursorPosCallBack = false;
    windowSizeCallBack = false;
}

GLFWcursorposfun InputReceiver::setCursorPosCallback(void (*callback)(GLFWwindow *, double, double))
{
    return glfwSetCursorPosCallback(window, callback);
}

GLFWwindowsizefun InputReceiver::setWindowSizeCallback(void (*callback)(GLFWwindow *, int width, int height))
{
    return glfwSetWindowSizeCallback(window, callback);
}

void InputReceiver::setCursorPos(double xpos, double ypos)
{
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width * xpos, height * ypos);
}

glm::vec2 InputReceiver::getCursorPos() noexcept
{
    glfwGetCursorPos(window, &posX, &posY);

    return {posX, posY};
}

glm::vec2 InputReceiver::getCursorPosRelative() noexcept
{
    glfwGetWindowSize(window, &width, &height);
    glfwGetCursorPos(window, &posX, &posY);
    return {float(posX / width), float(posY / height)};
}

void InputReceiver::pollEvents()
{
    glfwPollEvents();
}

bool InputReceiver::isKeyPressed(int key)
{
    if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST)
        return glfwGetKey(window, key) == GLFW_PRESS;
    else
        return glfwGetMouseButton(window, key) == GLFW_PRESS;
}

int InputReceiver::isKeyPressOneTime(int key)
{
    if (not keysPressed[key] && isKeyPressed(key)) // PRESS ONE TIME
    {
        keysPressed[key] = true;
        return ONETIME;
    }
    else if (keysPressed[key] && not isKeyPressed(key)) // RELEASE
    {
        keysPressed[key] = false;
        return RELEASED;
    }
    return -1;
}

std::vector<std::pair<int, InputPressType>> InputReceiver::getKeysPressed(std::vector<std::pair<int, InputPressType>> const &checkKeys)
{
    std::vector<std::pair<int, InputPressType>> keysPressedInput;
    for (std::size_t i = 0; i < checkKeys.size(); i++)
    {
        if (checkKeys[i].second == ONETIME && isKeyPressOneTime(checkKeys[i].first) == ONETIME)
        {
            keysPressedInput.emplace_back(checkKeys[i]);
        }
        else if (checkKeys[i].second == PRESSED && isKeyPressed(checkKeys[i].first))
        {
            keysPressedInput.emplace_back(checkKeys[i]);
        }
        else if (checkKeys[i].second == RELEASED && isKeyPressOneTime(checkKeys[i].first) == RELEASED)
        {
            keysPressedInput.emplace_back(checkKeys[i]);
        }
    }
    return keysPressedInput;
}

void InputReceiver::setVisible(bool vis)
{
    if (vis)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void InputReceiver::changeFullScreen(int type)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);
    float screenWidth = float(videoMode->width);
    float screenHeight = float(videoMode->height);

    if (type == 0)
    {
        glfwSetWindowMonitor(window, monitor, 0, 0, int(screenWidth), int(screenHeight), GLFW_DONT_CARE);
    }
    else
    {
        glfwSetWindowMonitor(window, nullptr, 0, 0, int(screenWidth / 1.5f), int(screenHeight / 1.5f), GLFW_DONT_CARE);
    }
}

glm::vec2 InputReceiver::getMaxScreenSizes()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);
    float m_width = float(videoMode->width);
    float m_height = float(videoMode->height);

    return glm::vec2{m_width, m_height};
}

void InputReceiver::setWindowSize(glm::vec2 size)
{
    glfwSetWindowSize(window, int(size.x), int(size.y));
}

void InputReceiver::setStandardCursorShape(MouseCursorShape cursorShape)
{
    glfwSetCursor(window, glfwCreateStandardCursor(cursorShape));
}

glm::vec2 InputReceiver::getScreenSizes() const noexcept
{
    return glm::vec2{width, height};
}