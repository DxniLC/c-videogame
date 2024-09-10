#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/vector_angle.hpp>

struct Transform
{
    explicit Transform() = default;

    glm::mat4 calculateTransformMatrix(float deltaTime);

    void setPosition(float x, float y, float z);
    void setPosition(glm::vec3 vector);

    void addPosition(float x, float y, float z);
    void addPosition(glm::vec3 vector);

    void setPositionInterpolated(glm::vec3 direction, float distance, float speed);

    void setRotate(float x, float y, float z);
    void setRotate(glm::vec3 vector);

    glm::vec3 setRotate(glm::vec3 currentDirection, glm::vec3 newDirection);
    float setRotate(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis);

    void addRotate(float x, float y, float z);
    void addRotate(glm::vec3 vector);

    float addRotate(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis = glm::vec3(0.0f));

    void setRotateInterpolated(glm::vec3 currentDirection, glm::vec3 newDirection, float speed, glm::vec3 axis = glm::vec3(0.0f));

    void setScale(float x, float y, float z);
    void setScale(glm::vec3 vector);

    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    bool need_update{false};
    bool need_scale_update{false};
    bool interpolated_need_update{false};

    static float getAngle(glm::vec3 currentDirection, glm::vec3 newDirection, glm::vec3 axis = glm::vec3(0.0f));

private:
    // Interpolate Translate
    bool translating{false};
    glm::vec3 directionTarget{0.0f};
    float distance{0.0f};
    float translateSpeed{0.0f};
    float currentStep{0.0f};

    // Interpolate Rotate
    bool rotating{false};
    float currentAngle{0.0f};
    glm::vec3 rotationAxis{0.0f};
    float targetAngle{0.0f};
    float rotationSpeed{0.0f};

    float angleLimit(float angle);

    void interpolatingUpdate(float &deltaTime);
};
