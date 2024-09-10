
#include "Animator.hpp"

#include <iostream>

Animator::Animator(ResourceAnimation *animation)
    : currentAnimation{animation}
{
    finalBoneTransforms.reserve(100);

    for (int i = 0; i < 100; i++)
        finalBoneTransforms.emplace_back(glm::mat4(1.0f));
}

void Animator::updateAnimation(float deltaTime)
{

    if (targetAnimation)
    {
        BlendTime += deltaTime;
        BlendFactor = glm::clamp(BlendTime / TransitionDuration, 0.0f, 1.0f);
        BlendAnimation(&currentAnimation->getRootNode(), &targetAnimation->getRootNode(), glm::mat4(1.0f), BlendFactor);

        if (BlendFactor >= 1.0f)
        {
            CompleteCurrentAnimation = false;
            currentAnimation = targetAnimation;
            targetAnimation = nullptr;
            currentTime = 0.0f;
            BlendTime = 0.0f;
        }
    }
    else if (currentAnimation)
    {
        currentTime += (currentAnimation->getTicksPerSecond() * deltaTime) * speed;

        if (CompleteCurrentAnimation && currentTime >= currentAnimation->getDuration())
        {
            CompleteCurrentAnimation = false;
            currentAnimation = nullptr;
            return;
        }

        currentTime = std::fmod(currentTime, currentAnimation->getDuration());
        calculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
    }
}

void Animator::playAnimation(ResourceAnimation *animation, bool CompleteAnimation, float TransitionDuration, float speed)
{
    if (not currentAnimation)
        currentAnimation = animation;
    else if (CompleteAnimation || (not CompleteCurrentAnimation && animation != currentAnimation))
    {
        if (CompleteAnimation)
            currentTime = 0.0f;

        CompleteCurrentAnimation = CompleteAnimation;
        if (TransitionDuration != 0.0f)
        {

            this->TransitionDuration = TransitionDuration;
            targetAnimation = animation;
        }
        else
            currentAnimation = animation;

        // currentTime = 0.0f; estaba aqui antes
    }
    this->speed = speed;
}

void Animator::calculateBoneTransform(const AnimationNodeData *node, glm::mat4 &&parentTransform)
{

    Bone *currentBone = currentAnimation->getBone(node->name);

    glm::mat4 nodeTransform = node->transform;

    if (currentBone)
    {
        currentBone->update(currentTime);
        nodeTransform = currentBone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto &boneInfoMap = currentAnimation->getBoneInfoMap();

    auto itr = boneInfoMap.find(node->name);

    if (itr != boneInfoMap.end())
    {
        finalBoneTransforms[std::size_t(itr->second.id)] = globalTransformation * itr->second.offset;
    }

    for (std::size_t i = 0; i < node->children.size(); i++)
        calculateBoneTransform(&node->children[i], std::move(globalTransformation));
}

void Animator::BlendAnimation(const AnimationNodeData *currentNode, const AnimationNodeData *targetNode, glm::mat4 &&parentTransform, float blendFactor)
{
    Bone *currentBone = currentAnimation->getBone(currentNode->name);

    glm::mat4 currentTransform = currentNode->transform;

    if (currentBone)
    {
        currentBone->update(currentTime);
        currentTransform = currentBone->getLocalTransform();
    }

    Bone *targetBone = targetAnimation->getBone(targetNode->name);

    glm::mat4 targetTransform = targetNode->transform;

    if (targetBone)
    {
        targetBone->update(0);
        targetTransform = targetBone->getLocalTransform();
    }

    // TRANSLATE

    glm::vec3 BlendedTranslation = (1.0f - blendFactor) * currentTransform[3] + targetTransform[3] * blendFactor;

    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0f), BlendedTranslation);

    // ROTATION
    const glm::quat &currentRotation = glm::quat_cast(currentTransform);
    const glm::quat &targetRotation = glm::quat_cast(targetTransform);

    const glm::quat BlendedRot = glm::slerp(currentRotation, targetRotation, blendFactor);
    // toMat4
    glm::mat4 RotationMatrix = glm::mat4_cast(BlendedRot);

    // FINAL TRANSFORM
    glm::mat4 FinalTransform = TranslationMatrix * RotationMatrix;

    glm::mat4 globalTransformation = parentTransform * FinalTransform;

    auto &boneInfoMap = currentAnimation->getBoneInfoMap();

    auto itr = boneInfoMap.find(currentNode->name);

    if (itr != boneInfoMap.end())
    {
        finalBoneTransforms[std::size_t(itr->second.id)] = globalTransformation * itr->second.offset;
    }

    for (std::size_t i = 0; i < currentNode->children.size() && i < targetNode->children.size(); i++)
        BlendAnimation(&currentNode->children[i], &targetNode->children[i], std::move(globalTransformation), blendFactor);
}