#pragma once

#include "InputControl.hpp"
#include <GLFW/glfw3.h>

#include <ecs/util/typealiases.hpp>

#include <glm/vec3.hpp>

struct ClimbControl : InputControl
{
    explicit ClimbControl(TNode *player, glm::vec3 direction, glm::vec2 minLimits, glm::vec2 maxLimits);

    void doActions(ECS::Entity &entity, ECS::EntityManager &entityManager) override final;

private:
    TNode *nodePlayer{nullptr}; // Node Player

    ECS::Entity *entity_input{nullptr}; // Entity Player

    EventManager *eventManager{nullptr};

    glm::vec3 WallFaceDirection{0.0f, 0.0f, 0.0f};

    glm::vec2 minLimits{}, maxLimits{};

    bool canColide{false};

    const float inputForce{75};
    glm::vec3 new_force{};

    void MoveUp();
    void MoveLeft();
    void MoveDown();
    void MoveRight();
    void Fall();

    void Pause();

    void ClimbWall();

    inline static float lastX{};
    inline static float lastY{};
    inline static ECS::Entity *entity_cam{nullptr}; // Entity Cam

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
};
