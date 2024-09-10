
#pragma once

#include <engines/resource/TResource.hpp>

#include <string>

#include <map>
#include <unordered_map>

#include <engines/graphic/SGEngine/model/animation/Bone.hpp>
#include <engines/graphic/SGEngine/model/animation/BoneInfo.hpp>

struct AnimationNodeData
{
    glm::mat4 transform;
    std::string name;
    std::vector<AnimationNodeData> children;
};

struct ResourceModel;
struct ResourceAnimation : TResource
{
    ResourceAnimation() = default;
    explicit ResourceAnimation(const std::string &path, ResourceModel *model);

    void loadFile([[maybe_unused]] const char *path) override final{};

    Bone *getBone(const std::string &name);

    inline constexpr float getTicksPerSecond() const noexcept { return TicksPerSecond; }
    inline constexpr float getDuration() const noexcept { return duration; }
    inline const AnimationNodeData &getRootNode() { return rootNode; }
    inline const auto &getBoneInfoMap() const noexcept { return boneInfoMap; }

private:
    float duration;
    float TicksPerSecond;
    std::map<std::string, BoneInfo> boneInfoMap;
    std::unordered_map<std::string, Bone> bones;
    AnimationNodeData rootNode;

    void readMissingBones(const aiAnimation *animation, ResourceModel &model);

    void readHierarchyData(AnimationNodeData &dest, const aiNode *src);
};