
#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <vector>

struct KeyPosition
{
    KeyPosition(glm::vec3 position, float timeStamp) : position{position}, timeStamp{timeStamp} {}

    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    KeyRotation(glm::quat orientation, float timeStamp) : orientation{orientation}, timeStamp{timeStamp} {}

    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    KeyScale(glm::vec3 scale, float timeStamp) : scale{scale}, timeStamp{timeStamp} {}

    glm::vec3 scale;
    float timeStamp;
};

struct Bone
{
    explicit Bone(const std::string &name, int ID, const aiNodeAnim *channel);

    void update(float deltaTime);

    constexpr glm::mat4 getLocalTransform() const noexcept { return localTransform; };

    const std::string &getBoneName() const noexcept { return name; };
    constexpr int getBoneID() const noexcept { return ID; };

    int getPositionIndex(float animationTime);
    int getRotationIndex(float animationTime);
    int getScaleIndex(float animationTime);

private:
    std::vector<KeyPosition> Positions;
    std::vector<KeyRotation> Rotations;
    std::vector<KeyScale> Scales;

    glm::mat4 localTransform{1.0f};
    std::string name;
    int ID;

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScale(float animationTime);
};