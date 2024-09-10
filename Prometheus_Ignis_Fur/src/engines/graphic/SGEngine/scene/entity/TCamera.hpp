#pragma once

#include "TEntity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <memory>

#include <engines/graphic/SGEngine/model/SkyBox.hpp>

enum Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default Values
const float INITYAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.f;
const float SENSITIVITY = 4.f;
const float ZOOM = 45.0f;

struct CameraParams
{
    glm::vec3 position{};
    // euler Angles
    float Yaw{INITYAW};
    float Pitch{PITCH};
    // camera options
    float MovementSpeed{SPEED};
    float MouseSensitivity{SENSITIVITY};
    float Zoom{ZOOM};

    glm::vec3 vectorUP{0, 1, 0};
};

struct TCamera : TEntity
{

    struct FocusDirection
    {
        float X_angle;
        float Y_angle;
    };

    TCamera(CameraParams params);

    TCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = INITYAW, float pitch = PITCH);

    TCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    void update() override final;

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspectRatio);

    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float ZNear{0.1f};
    float ZFar{200.0f};

    // void setRotation(float x_degree, float y_degree, float z_degree = 0);
    void setPosition(float x_pos, float y_pos, float z_pos);
    // void setTarget(glm::vec3 target);

    glm::vec3 setTranslate(Movement direction, float deltaTime = 0.02f);

    void setRotation(glm::vec3 rotation);
    void setRotation(float x_degree, float y_degree, float z_degree);

    [[nodiscard]] const FocusDirection getAngles() const noexcept;
    std::unique_ptr<SkyBox> skyBox{nullptr};

private:
    void updateVectors();
};