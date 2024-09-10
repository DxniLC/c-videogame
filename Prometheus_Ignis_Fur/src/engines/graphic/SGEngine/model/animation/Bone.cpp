
#include "Bone.hpp"

#include <engines/graphic/SGEngine/util/Assimp_GLM.hpp>

Bone::Bone(const std::string &name, int ID, const aiNodeAnim *channel)
    : name{name}, ID{ID}
{

    Positions.reserve(channel->mNumPositionKeys);
    for (unsigned int positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++)
    {
        auto &positionKey = channel->mPositionKeys[positionIndex];
        Positions.emplace_back(AssimpGLMHelper::aiVector3ToGLM(positionKey.mValue), positionKey.mTime);
    }

    Rotations.reserve(channel->mNumRotationKeys);
    for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; rotationIndex++)
    {
        auto &rotationKey = channel->mRotationKeys[rotationIndex];
        Rotations.emplace_back(AssimpGLMHelper::aiQuatToGLM(rotationKey.mValue), rotationKey.mTime);
    }

    Scales.reserve(channel->mNumScalingKeys);
    for (unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; scaleIndex++)
    {
        auto &scaleKey = channel->mScalingKeys[scaleIndex];
        Scales.emplace_back(AssimpGLMHelper::aiVector3ToGLM(scaleKey.mValue), scaleKey.mTime);
    }
}

void Bone::update(float animationTime)
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScale(animationTime);
    localTransform = translation * rotation * scale;
}

int Bone::getPositionIndex(float animationTime)
{
    for (std::size_t index = 0; index < Positions.size() - 1; ++index)
    {
        if (animationTime < Positions[index + 1].timeStamp)
            return int(index);
    }
    assert(0);
    return -1;
}

int Bone::getRotationIndex(float animationTime)
{
    for (std::size_t index = 0; index < Rotations.size() - 1; ++index)
    {
        if (animationTime < Rotations[index + 1].timeStamp)
            return int(index);
    }
    assert(0);
    return -1;
}

int Bone::getScaleIndex(float animationTime)
{
    for (std::size_t index = 0; index < Scales.size() - 1; ++index)
    {
        if (animationTime < Scales[index + 1].timeStamp)
            return int(index);
    }
    assert(0);
    return -1;
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
    if (Positions.size() == 1)
        return glm::translate(glm::mat4(1.0f), Positions[0].position);

    std::size_t p0Index = std::size_t(getPositionIndex(animationTime));
    std::size_t p1Index = std::size_t(p0Index + 1);

    float scaleFactor = getScaleFactor(Positions[p0Index].timeStamp, Positions[p1Index].timeStamp, animationTime);

    glm::vec3 finalPosition = glm::mix(Positions[p0Index].position, Positions[p1Index].position, scaleFactor);

    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
    if (Rotations.size() == 1)
    {
        auto rotation = glm::normalize(Rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    std::size_t p0Index = std::size_t(getRotationIndex(animationTime));
    std::size_t p1Index = std::size_t(p0Index + 1);

    float scaleFactor = getScaleFactor(Rotations[p0Index].timeStamp, Rotations[p1Index].timeStamp, animationTime);

    glm::quat finalRotation = glm::slerp(Rotations[p0Index].orientation, Rotations[p1Index].orientation, scaleFactor);

    finalRotation = glm::normalize(finalRotation);

    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScale(float animationTime)
{
    if (Scales.size() == 1)
        return glm::scale(glm::mat4(1.0f), Scales[0].scale);

    std::size_t p0Index = std::size_t(getScaleIndex(animationTime));
    std::size_t p1Index = std::size_t(p0Index + 1);

    float scaleFactor = getScaleFactor(Scales[p0Index].timeStamp, Scales[p1Index].timeStamp, animationTime);

    glm::vec3 finalScale = glm::mix(Scales[p0Index].scale, Scales[p1Index].scale, scaleFactor);

    return glm::scale(glm::mat4(1.0f), finalScale);
}