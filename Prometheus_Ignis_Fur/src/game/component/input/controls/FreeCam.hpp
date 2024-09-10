#pragma once

#include "InputControl.hpp"
#include <GLFW/glfw3.h>

struct FreeCam : InputControl
{
    explicit FreeCam(TNode *cameraActived);

    void doActions(ECS::Entity &entity, ECS::EntityManager &entityManager) override final;

private:
    TNode *camera{nullptr};

    void MoveFront();
    void MoveLeft();
    void MoveBack();
    void MoveRight();
    void MoveUp();
    void MoveDown();

    inline static float lastX{};
    inline static float lastY{};
    inline static bool firstMouse{true};

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
};
