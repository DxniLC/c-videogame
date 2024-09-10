#pragma once

#include "InputControl.hpp"
#include <GLFW/glfw3.h>
#include <ecs/util/typealiases.hpp>

#include <glm/vec3.hpp>

struct BoxControl : InputControl
{
    explicit BoxControl(TNode *player, EntityKey boxKey, TNode *nodeBox, glm::vec3 direction);

    void doActions(ECS::Entity &entity, ECS::EntityManager &entityManager) override final;

private:
    TNode *nodeBox{nullptr};    // Node Box
    TNode *nodePlayer{nullptr}; // Node Player

    ECS::Entity *entity_input{nullptr}; // Entity Player
    ECS::Entity *entity_box{nullptr};

    EventManager *eventManager{nullptr};

    EntityKey boxKey{}; // Box key
    glm::vec3 direction_to_move{0.0f, 0.0f, 0.0f};

    bool canColide{false};

    int currentKey{0};
    int lastKeyPressed{0};

    const float inputForce{100};
    glm::vec3 new_force{};

    void PushPull();

    void LeftBox();
    void Pause();
    void ShowDebug();

    inline static float lastX{};
    inline static float lastY{};
    inline static ECS::Entity *entity_cam{nullptr}; // Entity Cam

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
    bool iPushed{false};

};
