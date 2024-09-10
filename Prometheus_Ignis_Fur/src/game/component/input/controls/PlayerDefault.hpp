#pragma once

#include "InputControl.hpp"
#include <GLFW/glfw3.h>
#include <chrono>

#include <glm/vec3.hpp>

struct ParticleGenerator;
struct PlayerDefault : public InputControl
{
    explicit PlayerDefault(TNode *player);

    void doActions(ECS::Entity &entity, ECS::EntityManager &entityManager) override final;

private:
    TNode *nodePlayer{nullptr};
    ECS::Entity *entity_input{nullptr};
    ECS::EntityManager *entityManager{nullptr};

    float inputForce{0};
    const float acceleration{10.f};

    glm::vec3 camFront{};
    glm::vec3 camRight{};

    ParticleGenerator *particleGen{nullptr};

    void setOrientation(glm::vec3 const &newDirection);

    glm::vec3 new_force{};
    float extraForce{1};

    std::chrono::system_clock::time_point mStartTime{};
    std::chrono::duration<float> mDeltaTime{};

    EventManager *eventManager{nullptr};

    bool IsListened{false};

    double timeLastJump{std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count()};
    const float JumpColdDown{0.3f};
    bool HasJumped{false};

    float AnimationColdDown{0.35f};
    bool HasDashed{false};

    void MoveFront();
    void MoveLeft();
    void MoveBack();
    void MoveRight();
    void ChangePower();
    void UsePower();
    void Jump();
    void Run();
    void Dash();
    void Interact();
    void Teleport(glm::vec3 postp);

    void chargeBow();
    float inputBowForce{0.f};
    unsigned int currentFrames{0};

    void Prueba();

    // ARROW
    bool IsAiming{false};

    bool IsFalling{false};

    void FireArrow();
    void AimCamera();
    void GoCameraNormal();

    void resetCamera();

    void goStealthMode(bool active);
    bool IsStealth{false};
    

    void Pause();
    void ShowDebug();

    inline static float lastX{};
    inline static float lastY{};

    inline static ECS::Entity *entity_cam{nullptr};

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
};
