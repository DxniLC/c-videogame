
#include "Transform.hpp"

glm::mat4 Transform::calculateTransformMatrix(float deltaTime)
{

    if (interpolated_need_update)
        interpolatingUpdate(deltaTime);

    glm::mat4 matrixTransform = glm::mat4(1.0f);

    matrixTransform = glm::translate(matrixTransform, position);
    matrixTransform = glm::rotate(matrixTransform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    matrixTransform = glm::rotate(matrixTransform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    matrixTransform = glm::rotate(matrixTransform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    matrixTransform = glm::scale(matrixTransform, scale);

    return matrixTransform;
}

void Transform::setPosition(float x, float y, float z)
{
    glm::vec3 newPosition{x, y, z};
    if (position != newPosition && not translating)
    {
        position = newPosition;
        need_update = true;
    }
}
void Transform::setPosition(glm::vec3 vector)
{
    setPosition(vector.x, vector.y, vector.z);
}

void Transform::addPosition(float x, float y, float z)
{
    glm::vec3 newPosition{x, y, z};
    position += newPosition;
    need_update = true;
}

void Transform::addPosition(glm::vec3 vector)
{
    addPosition(vector.x, vector.y, vector.z);
}

void Transform::setPositionInterpolated(glm::vec3 direction, float distance, float speed)
{
    if (distance != 0.0f)
    {
        currentStep = 0;
        directionTarget = glm::normalize(direction);
        this->distance = distance;
        translateSpeed = speed;
        translating = true;
        interpolated_need_update = true;
    }
}

void Transform::setRotate(float x, float y, float z)
{
    glm::vec3 newRotation{x, y, z};
    if (rotation != newRotation && not rotating)
    {
        rotation = newRotation;

        rotation.x = angleLimit(rotation.x);
        rotation.y = angleLimit(rotation.y);
        rotation.z = angleLimit(rotation.z);

        need_update = true;
    }
}

void Transform::setRotate(glm::vec3 vector)
{
    setRotate(vector.x, vector.y, vector.z);
}

glm::vec3 Transform::setRotate(glm::vec3 currentDirection, glm::vec3 newDirection)
{
    glm::quat rotation = glm::rotation(currentDirection, newDirection);
    glm::vec3 euler = glm::eulerAngles(rotation);
    euler = glm::degrees(euler);
    setRotate(euler);
    return euler;
}

float Transform::setRotate(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis)
{
    float angle = getAngle(currentDirection, newDirection, axis);
    setRotate(axis * angle);
    return angle;
}

void Transform::addRotate(float x, float y, float z)
{
    glm::vec3 newRotation{x, y, z};
    rotation += newRotation;

    rotation.x = angleLimit(rotation.x);
    rotation.y = angleLimit(rotation.y);
    rotation.z = angleLimit(rotation.z);

    need_update = true;
}

void Transform::addRotate(glm::vec3 vector)
{
    addRotate(vector.x, vector.y, vector.z);
}

float Transform::addRotate(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis)
{
    float angle = getAngle(currentDirection, newDirection, axis);
    addRotate(axis * angle);
    return angle;
}

void Transform::setRotateInterpolated(glm::vec3 currentDirection, glm::vec3 newDirection, float speed, glm::vec3 axis)
{
    float angle = getAngle(currentDirection, newDirection, axis);
    if (not(angle < 2.0f && angle > -2.0f))
    {
        currentAngle = 0.0f;
        rotationAxis = (axis != glm::vec3(0.0f)) ? axis : glm::vec3(1.0f);
        targetAngle = angle;
        rotationSpeed = (targetAngle >= 0.0f) ? speed : -speed;
        rotating = true;
        interpolated_need_update = true;
    }
}

void Transform::setScale(float x, float y, float z)
{
    glm::vec3 newScale{x, y, z};
    if (scale != newScale)
    {
        scale = newScale;
        need_update = true;
        need_scale_update = true;
    }
}
void Transform::setScale(glm::vec3 vector)
{
    setScale(vector.x, vector.y, vector.z);
}

float Transform::getAngle(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis)
{
    float angle = glm::degrees(glm::angle(currentDirection, newDirection));

    if (axis != glm::vec3(0.0f) && glm::dot(glm::cross(currentDirection, newDirection), axis) < 0.0f)
        angle = -angle;

    return angle;
}

float Transform::angleLimit(float angle)
{
    angle = std::fmod(angle, 360.f);

    if (angle > 180.f)
        angle -= 360.f;
    else if (angle < -180.f)
        angle += 360.f;

    return angle;
}

void Transform::interpolatingUpdate(float &deltaTime)
{
    float interpolateSpeed{};
    if (translating)
    {
        interpolateSpeed = translateSpeed * deltaTime;
        currentStep += interpolateSpeed;
        if (currentStep >= distance)
        {
            translating = false;
        }

        addPosition(directionTarget * interpolateSpeed);
    }
    if (rotating)
    {
        interpolateSpeed = rotationSpeed * deltaTime;
        currentAngle += interpolateSpeed;
        if (std::abs(currentAngle) >= std::abs(targetAngle))
        {
            interpolateSpeed = currentAngle - targetAngle;
            rotating = false;
        }

        addRotate(rotationAxis * interpolateSpeed);
    }

    if (!rotating && !translating)
        interpolated_need_update = false;
}