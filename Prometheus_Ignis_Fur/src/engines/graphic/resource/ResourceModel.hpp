#pragma once

#include <engines/resource/TResource.hpp>

#include <engines/graphic/SGEngine/model/Mesh.hpp>
#include <memory>
#include <map>
#include <unordered_map>
#include <assimp/material.h>
#include <engines/graphic/resource/ResourceTexture.hpp>
#include <engines/graphic/resource/ResourceMaterial.hpp>

#include <engines/graphic/SGEngine/model/animation/BoneInfo.hpp>

// Forward declaration ASSIMP
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
struct ResourceModel : TResource
{
    explicit ResourceModel() = default;

    void loadFile(const char *path) override final;

    void loadFileWithInstances(const char *path, std::vector<glm::mat4> &matricesTransforms);

    void draw(glm::mat4 matrixTransform) override final;

    void createCube(glm::vec3 const &size);

    auto &getBoneInfoMap() noexcept { return boneInfoMap; };
    int &getBoneCount() noexcept { return boneCounter; };

    void setTexture(ResourceTexture *path);
    glm::vec3 dimension{0.0f};

    void addLightToRender(LightParams &light);

    void updateBones(std::vector<glm::mat4> const &finalBoneTransforms);

    constexpr bool IsInstance() const noexcept { return HasInstances; };

    std::vector<std::unique_ptr<Mesh>> meshes;

    void clearLightsToRender();

private:
    std::string pathMesh{};
    Mesh meshResult{};
    // Assimp loads
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);
    std::pair<float, float> limitModelX{__FLT_MAX__, -__FLT_MAX__};
    std::pair<float, float> limitModelY{__FLT_MAX__, -__FLT_MAX__};
    std::pair<float, float> limitModelZ{__FLT_MAX__, -__FLT_MAX__};

    ResourceTexture *loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType textureType);

    // Skeletal
    bool HasBones{false};
    std::map<std::string, BoneInfo> boneInfoMap;
    int boneCounter{0};

    void setVertexBoneToDefault(Vertex &vertex);
    void setVertexBone(Vertex &vertex, int const &boneID, float const &weight);
    void getBoneWeight4Vertices(std::vector<Vertex> &vertices, aiMesh *mesh);

    bool HasInstances{false};
};