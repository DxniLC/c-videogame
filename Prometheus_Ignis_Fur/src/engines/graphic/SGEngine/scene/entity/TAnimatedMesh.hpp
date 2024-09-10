#pragma once

#pragma once

#include "TEntity.hpp"

#include <engines/graphic/SGEngine/model/animation/Animator.hpp>

#include <unordered_map>
#include <chrono>

struct SGEngine;
struct ResourceModel;
struct ResourceTexture;
struct TAnimatedMesh : public TEntity
{
    using SecondsFP = std::chrono::duration<float>;
    using AnimationID = uint8_t;

    friend SGEngine;

    explicit TAnimatedMesh(ResourceModel *model) : mesh(model) {}

    void update() override final;

    void setTexture(ResourceTexture *texture);

    void insertAnimation(AnimationID animationID, ResourceAnimation *animation);

    void playAnimation(AnimationID animationID, bool CompleteAnimation = false, float TransitionDuration = 0.20f, float speed = 1.0f);

private:
    ResourceModel *mesh{nullptr};
    ResourceAnimation *currentAnimation{nullptr};

    std::unordered_map<AnimationID, ResourceAnimation *> animationsMap;

    Animator animator{nullptr};
};