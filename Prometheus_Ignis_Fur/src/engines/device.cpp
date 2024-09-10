
#include "device.hpp"

#include "graphic/SGEngine/SGEngine.hpp"

#include "audio/FMODSystem.hpp"

#include <GLAD/src/glad.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

SGDevice::SGDevice(int width_, int height_)
{
    if (not Initialized)
    {
        width = width_;
        height = height_;

        initWindow();

        GraphicEngine::setWindow(*window.get());

        graphic = std::make_unique<SGEngine>(width, height);

        audio = std::make_unique<FMODSystem>();
        audio->initialize();

        receiver = std::make_unique<InputReceiver>(*window.get());

        Initialized = true;
    }
};

void SGDevice::initWindow()
{
    glfwSetErrorCallback([](auto error, auto description)
                         {
            fprintf(stderr, "Glfw Error %d: %s\n", error, description);
            throw std::runtime_error ("GLFW Error"); });
    if (!glfwInit())
        throw std::runtime_error("Error on GLFW Init");
    // return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

    window = WindowManaged(glfwCreateWindow(width, height, "Game", nullptr, nullptr), destroyWindow);
    if (!window)
        throw std::runtime_error("GLFW Error on creating Window");

    glfwMakeContextCurrent(window.get());

    // VSync Enabled
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    // No Resize Window
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Global settings.
    // ZBuffer
    glEnable(GL_DEPTH_TEST);

    // Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSetWindowAspectRatio(window.get(), 16, 9);

    glfwSetWindowAspectRatio(window.get(), width, height);
    glfwSetWindowSize(window.get(), width, height);

    // si reescalamos ventana, hay que actualizar viewport
    glViewport(0, 0, width, height);

    // glfwSetWindowSizeLimits(window.get(), 640, 360, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void SGDevice::destroyWindow(GLFWwindow *w)
{
    std::cout << "\n*** DESTRUYO VENTANA ***\n";
    if (w)
    {
        glfwDestroyWindow(w);
    }
    // MEMORY LEAKS
    glfwTerminate();

    Initialized = false;
}

bool SGDevice::isRun() const noexcept
{
    return not bool(glfwWindowShouldClose(window.get()));
}

InputReceiver *SGDevice::getInputReceiver()
{
    return receiver.get();
}

GraphicEngine *SGDevice::getGraphicEngine()
{
    return graphic.get();
}

AudioEngine *SGDevice::getAudioEngine()
{
    return audio.get();
}

GLFWwindow &SGDevice::getWindow()
{
    return *window.get();
}

double SGDevice::getTime() const noexcept
{
    auto current_time = std::chrono::system_clock::now();
    auto duration_in_sec = std::chrono::duration<double>(current_time.time_since_epoch());
    return duration_in_sec.count();
}
