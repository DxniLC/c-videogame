#pragma once

#include <engines/graphic/resource/ResourceAnimation.hpp>

struct Animator
{
    explicit Animator(ResourceAnimation *aniamtion);

    void updateAnimation(float deltaTime);

    void playAnimation(ResourceAnimation *animation, bool CompleteAnimation = false, float TransitionDuration = 0.20f, float speed = 1.0f);

    void calculateBoneTransform(const AnimationNodeData *node, glm::mat4 &&parentTransform);

    std::vector<glm::mat4> const &getFinalBoneTransforms() { return finalBoneTransforms; }

private:
    std::vector<glm::mat4> finalBoneTransforms;
    ResourceAnimation *currentAnimation{nullptr};
    float currentTime{0.0f};
    bool CompleteCurrentAnimation{false};

    ResourceAnimation *targetAnimation{nullptr};
    float BlendTime{0.0f};
    float BlendFactor{0.0f};
    float TransitionDuration{0.20f};

    float speed{1.0f};

    void BlendAnimation(const AnimationNodeData *currentNode, const AnimationNodeData *targetNode, glm::mat4 &&parentTransform, float blendFactor);
};
