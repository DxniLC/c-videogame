#include "TAnimatedMesh.hpp"

#include <engines/graphic/resource/ResourceModel.hpp>

#include <glm/mat4x4.hpp>

#include <GLFW/glfw3.h>

void TAnimatedMesh::update()
{
    animator.updateAnimation(deltaTime);
    auto &transforms = animator.getFinalBoneTransforms();
    mesh->updateBones(transforms);
    mesh->draw(matrix);
}

void TAnimatedMesh::setTexture(ResourceTexture *texture)
{
    mesh->setTexture(texture);
}

void TAnimatedMesh::insertAnimation(AnimationID animationID, ResourceAnimation *animation)
{
    animationsMap.emplace(animationID, animation);
}

void TAnimatedMesh::playAnimation(AnimationID animationID, bool CompleteAnimation, float TransitionDuration, float speed)
{
    auto itr = animationsMap.find(animationID);
    if (itr != animationsMap.end())
        animator.playAnimation(itr->second, CompleteAnimation, TransitionDuration, speed);
}
