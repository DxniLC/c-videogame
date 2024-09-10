#pragma once

#include <glm/glm.hpp>

enum LightType
{
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT
};

struct LightParams
{
    LightType type{LightType::DIRECTIONAL_LIGHT};
    glm::vec3 position{};

    // Light Properties
    glm::vec3 ambient{0.5f, 0.5f, 0.5f};
    glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 specular{1.0f, 1.0f, 1.0f};

    // Direction Light
    glm::vec3 direction{0.0f, -0.5f, -0.5f};

    // PointLight Attenuation
    float constant{1.0f};
    float linear{0.09f};
    float quadratic{0.032f};

    // SpotLight
    constexpr float getCutOffAngle() { return angleCutOff; }
    constexpr float &getCutOff() noexcept { return cutOff; }
    void setCutOff(float EulerAngle)
    {
        angleCutOff = EulerAngle;
        cutOff = glm::cos(glm::radians(EulerAngle));
    }
    constexpr float getOuterCutOffAngle() { return angleOuterCutOff; }
    constexpr float &getOuterCutOff() noexcept { return outerCutOff; }
    void setOuterCutOff(float EulerAngle)
    {
        outerCutOff = EulerAngle;
        outerCutOff = glm::cos(glm::radians(EulerAngle));
    }

    bool HasAttenuation{false};
    bool actived{true};

    float RangeLight{10.0f};

private:
    float angleCutOff{45.0f};
    float cutOff{glm::cos(glm::radians(angleCutOff))};
    float angleOuterCutOff{50.5f};
    float outerCutOff{glm::cos(glm::radians(angleOuterCutOff))};
};