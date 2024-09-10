
#include "TCamera.hpp"

#include <glm/glm.hpp>

#include <iostream>

TCamera::TCamera(CameraParams params) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(params.MovementSpeed), MouseSensitivity(params.MouseSensitivity), Zoom(params.Zoom)
{
    Position = params.position;
    WorldUp = params.vectorUP;
    Yaw = params.Yaw;
    Pitch = params.Pitch;
    updateVectors();
}

TCamera::TCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateVectors();
}

TCamera::TCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateVectors();
}

void TCamera::update()
{
    Position = glm::vec3(matrix[3]);
}

glm::mat4 TCamera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 TCamera::GetProjectionMatrix(float aspectRatio)
{
    return glm::perspective(glm::radians(Zoom), aspectRatio, ZNear, ZFar);
}

// ############################################################################################################################################################
// // // TODO: Mejorar rotacion y Target ya que no va vinculado al Yaw y Pitch y debería
// ############################################################################################################################################################

void TCamera::setRotation(glm::vec3 rotation)
{
    setRotation(rotation.x, rotation.y, rotation.z);
}

void TCamera::setRotation(float x_degree, float y_degree, [[maybe_unused]] float z_degree)
{

    Pitch = x_degree;
    Yaw = y_degree;

    float angle = std::fmod(Pitch, 360.f);

    if (angle < -180.f)
    {
        Pitch += 360;
    }
    else if (angle > 180.f)
    {
        Pitch -= 360;
    }

    angle = std::fmod(Yaw, 360.f);

    if (angle < -180)
        Yaw += 360;
    else if (angle > 180)
        Yaw -= 360;

    updateVectors();
}

// void TCamera::setTarget(glm::vec3 target)
// {
//     if (target != Position)
//     {
//         glm::vec3 direction = glm::normalize(target - Position);

//         glm::vec2 directionXZ(direction.x, direction.z);
//         glm::vec2 frontXZ(Front.x, Front.z);
//         float X_Rotate = acos(glm::dot(directionXZ, frontXZ) / (glm::length(directionXZ) * glm::length(frontXZ))) * 180.f / M_PI;

//         glm::vec2 directionXY(direction.x, direction.y);
//         glm::vec2 frontXY(Front.x, Front.y);
//         float Y_Rotate = acos(glm::dot(directionXY, frontXY) / (glm::length(directionXY) * glm::length(frontXY))) * 180.f / M_PI;

//         // glm::vec2 Front_XZ = {Front.x, Front.z};
//         // glm::vec2 Target_XZ = {direction.x, direction.z};
//         // float X_Rotate = acos(glm::dot(Front_XZ, Target_XZ) / (glm::length(Front_XZ) * glm::length(Target_XZ))) * (180.f / M_PI);
//         // glm::vec2 Front_YX = {Front.x, Front.y};
//         // glm::vec2 Target_YX = {direction.x, direction.y};
//         // float Y_Rotate = acos(glm::dot(Front_YX, Target_YX) / (glm::length(Front_YX) * glm::length(Target_YX))) * (180.f / M_PI);

//         std::cout << "\n\n\n GlobalPos: (" << target.x << ", " << target.y << ", " << target.z << ")";
//         std::cout << "\n CameraPos: (" << Position.x << ", " << Position.y << ", " << Position.z << ")";
//         std::cout << "\n Front: (" << Front.x << ", " << Front.y << ", " << Front.z << ")";
//         std::cout << "\n Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")";
//         std::cout << "\n Rotate: " << X_Rotate << ", " << Y_Rotate;
//         std::cout << "\n Yaw: " << Yaw << " /// Pitch: " << Pitch;

//         setRotation(X_Rotate, Y_Rotate);
//     }
// }

// ############################################################################################################################################################
// ############################################################################################################################################################
// ############################################################################################################################################################

const TCamera::FocusDirection TCamera::getAngles() const noexcept
{
    return {Yaw, Pitch};
}

void TCamera::setPosition(float x_pos, float y_pos, float z_pos)
{
    Position = {x_pos, y_pos, z_pos};
}

glm::vec3 TCamera::setTranslate(Movement direction, float deltaTime)
{
    switch (direction)
    {
    case FORWARD:
        Position += Front * MovementSpeed * deltaTime;
        break;
    case BACKWARD:
        Position -= Front * MovementSpeed * deltaTime;
        break;
    case LEFT:
        Position -= Right * MovementSpeed * deltaTime;
        break;
    case RIGHT:
        Position += Right * MovementSpeed * deltaTime;
        break;
    case UP:
        Position += Up * MovementSpeed * deltaTime;
        break;
    case DOWN:
        Position -= Up * MovementSpeed * deltaTime;
        break;
    }
    return Position;
}

// PRIVATE

void TCamera::updateVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = std::cos(glm::radians(Yaw + INITYAW)) * std::cos(glm::radians(Pitch));
    front.y = std::sin(glm::radians(Pitch));
    front.z = std::sin(glm::radians(Yaw + INITYAW)) * std::cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // std::cout << "\n\n\n FINAL FRONT: " << Front.x << ", " << Front.y << ", " << Front.z << "\n\n\n";

    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}