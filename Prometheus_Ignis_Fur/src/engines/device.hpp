#pragma once

#include "input.hpp"

#include <memory>
#include <iostream>
#include <chrono>

#include "graphic/GraphicEngine.hpp"

#include "audio/AudioEngine.hpp"

struct SGDevice
{
    explicit SGDevice(int width_ = 1280, int height_ = 720);

    SGDevice(const SGDevice &) = delete;            // constructor copia
    SGDevice(SGDevice &&) = delete;                 // constructor move
    SGDevice &operator=(const SGDevice &) = delete; // operador = cte
    SGDevice &operator=(SGDevice &&) = delete;      // operador = objeto

    bool isRun() const noexcept;

    InputReceiver *getInputReceiver();

    GLFWwindow &getWindow();

    GraphicEngine *getGraphicEngine();

    AudioEngine *getAudioEngine();

    double getTime() const noexcept;

private:
    int width{1280}, height{720};

    inline static bool Initialized{false};

    using WindowManaged = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)>;
    static void destroyWindow(GLFWwindow *w);
    WindowManaged window{nullptr, destroyWindow};
    std::unique_ptr<GraphicEngine> graphic{nullptr};

    std::unique_ptr<InputReceiver> receiver{nullptr};

    std::unique_ptr<AudioEngine> audio{nullptr};

    void initWindow();
};