
#include "ResourceAnimation.hpp"

#include "ResourceModel.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <engines/graphic/SGEngine/util/Assimp_GLM.hpp>

#include <iostream>

ResourceAnimation::ResourceAnimation(const std::string &path, ResourceModel *model)
{
    filename = path;

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

    assert(scene && scene->mRootNode);

    auto animation = scene->mAnimations[0];

    duration = float(animation->mDuration);
    TicksPerSecond = float(animation->mTicksPerSecond);

    readHierarchyData(rootNode, scene->mRootNode);
    readMissingBones(animation, *model);

    importer.FreeScene();
}

Bone *ResourceAnimation::getBone(const std::string &name)
{
    Bone *result{nullptr};

    auto itr = bones.find(name);
    if (itr != bones.end())
        result = &(itr->second);

    return result;
}

void ResourceAnimation::readMissingBones(const aiAnimation *animation, ResourceModel &model)
{
    auto &boneInfoMap = model.getBoneInfoMap();
    int &boneCount = model.getBoneCount();

    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        auto itr = boneInfoMap.find(boneName);

        if (itr == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.emplace(boneName, Bone{boneName, boneInfoMap[boneName].id, channel});
    }

    this->boneInfoMap = boneInfoMap;
}

void ResourceAnimation::readHierarchyData(AnimationNodeData &dest, const aiNode *src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transform = AssimpGLMHelper::aiMatrixToGLM(src->mTransformation);

    dest.children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        AnimationNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.emplace_back(newData);
    }
}