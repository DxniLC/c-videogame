#pragma once

#include <unordered_map>
#include <glm/vec2.hpp>
#include <vector>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct GLFWwindow;

typedef void (*GLFWcursorposfun)(GLFWwindow *window, double xpos, double ypos);
typedef void (*GLFWwindowsizefun)(GLFWwindow *window, int width, int height);

// GLFW MEMORY DIRECTIONS. Lo hacemos para no incluir la libreria de glfw en el hpp ya que fallaria GLAD por multiples inclusiones
enum MouseCursorShape : int
{
    ArrowCursor = 0x00036001,
    HandCursor = 0x00036004,
    IbeamCursor = 0x00036002,
    CrosshairCursor = 0x00036003,
    HResizeCursor = 0x00036005,
    VResizeCursor = 0x00036006
};

enum InputPressType : int
{
    ONETIME = 0,
    RELEASED,
    PRESSED,
};

struct InputReceiver
{

    explicit InputReceiver(GLFWwindow &w);

    ~InputReceiver();

    void clearCallbacks();

    GLFWcursorposfun setCursorPosCallback(void (*callback)(GLFWwindow *, double, double));

    GLFWwindowsizefun setWindowSizeCallback(void (*callback)(GLFWwindow *, int width, int height));

    void setCursorPos(double xpos, double ypos);

    glm::vec2 getCursorPos() noexcept;
    glm::vec2 getCursorPosRelative() noexcept;

    void pollEvents();

    bool isKeyPressed(int key);
    int isKeyPressOneTime(int key);

    std::vector<std::pair<int, InputPressType>> getKeysPressed(std::vector<std::pair<int, InputPressType>> const &checkKeys);

    void setVisible(bool vis);

    void setStandardCursorShape(MouseCursorShape cursorShape);

    void setWindowSize(glm::vec2 size);

    glm::vec2 getMaxScreenSizes();

    [[nodiscard]] glm::vec2 getScreenSizes() const noexcept;

    void changeFullScreen(int type);


private:
    std::unordered_map<int, bool> keysPressed;
    GLFWwindow *window{nullptr};
    int width{}, height{};
    double posX{}, posY{};
    bool cursorPosCallBack{false};
    bool windowSizeCallBack{false};
};