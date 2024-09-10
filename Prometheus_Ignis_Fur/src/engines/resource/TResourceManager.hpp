#pragma once

#include "TResource.hpp"

// Lista de todos los recursos
#include "../graphic/resource/ResourceShader.hpp"
#include "../graphic/resource/ResourceModel.hpp"
#include "../graphic/resource/ResourceAnimation.hpp"
#include "../graphic/resource/ResourceFont.hpp"
#include "../graphic/resource/ResourceTexture.hpp"
#include "../graphic/resource/ResourceMaterial.hpp"

#include <unordered_map>
#include <memory>

#include <filesystem>

struct TResourceManager
{
private:
    // MAP HASHES
    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ h2;
        }

        template <class T1, class T2>
        bool operator()(const std::pair<T1, T2> &a, const std::pair<T1, T2> &b) const
        {
            return a == b;
        }
    };

    struct glm_hash
    {
        std::size_t operator()(const glm::vec3 &vec) const
        {
            return std::hash<float>()(vec.x) ^ std::hash<float>()(vec.y) ^ std::hash<float>()(vec.z);
        }

        bool operator()(const glm::vec3 &a, const glm::vec3 &b) const
        {
            return a == b;
        }
    };

public:

    using MapShaderResources = std::unordered_map<std::pair<std::string, std::string>, std::unique_ptr<TResource>, pair_hash>;

    explicit TResourceManager() = default;

    ResourceModel *getModel(const char *path);

    ResourceModel *getInstancingModel(const char *path, std::vector<glm::mat4> &matricesTransforms);

    ResourceAnimation *getAnimation(const char *path, ResourceModel *model);

    ResourceTexture *getTexture(const char *path);

    ResourceFont *getFont(const char *path);

    ResourceMaterial *getMaterial(const char *path);

    ResourceTexture *getSkyBoxTexture(std::vector<std::string> const &faces);

    ResourceShader *getShader(const char *vertPath, const char *fragPath);

    [[nodiscard]] const MapShaderResources &getShaders() const noexcept;

    ResourceModel *getCubeResource(glm::vec3 &size);

    void clearResource(const char *path);

private:
    template <typename ResourceType>
    ResourceType *getResource(const char *filename)
    {
        static_assert(std::is_base_of<TResource, ResourceType>::value, "\n*** ERROR: Invalid Resource ***\n");
        assert(typeid(ResourceType) != typeid(TResource));
        assert(typeid(ResourceType) != typeid(ResourceShader));
        ResourceType *res{nullptr};
        auto itr = resources.find(filename);
        if (itr != resources.end())
        {
            res = dynamic_cast<ResourceType *>(itr->second.get());
        }
        else
        {
            auto new_itr = resources.emplace(filename, std::make_unique<ResourceType>());
            res = dynamic_cast<ResourceType *>(new_itr.first->second.get());
            if (res)
                res->loadFile(filename);
        }
        return res;
    }

    // Variables
    std::unordered_map<std::string, std::unique_ptr<TResource>> resources;

    MapShaderResources resources_shaders;

    // Cubes to visual tests
    std::unordered_map<glm::vec3, std::unique_ptr<TResource>, glm_hash> resources_cubes;

    // SkyBoxes
    std::unordered_map<std::string, std::unique_ptr<TResource>> skybox_resources;
};
