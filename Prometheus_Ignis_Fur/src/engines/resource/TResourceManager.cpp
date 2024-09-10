#include "TResourceManager.hpp"

ResourceModel *TResourceManager::getModel(const char *path)
{
    auto *model = getResource<ResourceModel>(path);
    model->clearLightsToRender();
    return model;
}

ResourceModel *TResourceManager::getInstancingModel(const char *path, std::vector<glm::mat4> &matricesTransforms)
{
    ResourceModel *model{nullptr};
    auto itr = resources.find(path);
    if (itr != resources.end())
    {
        // Only 4 Instances
        resources.erase(itr);

        // model = dynamic_cast<ResourceModel *>(itr->second.get());
        auto new_itr = resources.emplace(path, std::make_unique<ResourceModel>());
        model = dynamic_cast<ResourceModel *>(new_itr.first->second.get());
        if (model)
            model->loadFileWithInstances(path, matricesTransforms);

        // for (auto &mesh : model->meshes)
        //     mesh->updateInstances(matricesTransforms);
    }
    else
    {
        auto new_itr = resources.emplace(path, std::make_unique<ResourceModel>());
        model = dynamic_cast<ResourceModel *>(new_itr.first->second.get());
        if (model)
            model->loadFileWithInstances(path, matricesTransforms);
    }
    return model;
}

ResourceAnimation *TResourceManager::getAnimation(const char *path, ResourceModel *model)
{
    ResourceAnimation *res{nullptr};
    auto itr = resources.find(path);
    if (itr != resources.end())
    {
        res = dynamic_cast<ResourceAnimation *>(itr->second.get());
    }
    else
    {
        auto new_itr = resources.emplace(path, std::make_unique<ResourceAnimation>(path, model));
        res = dynamic_cast<ResourceAnimation *>(new_itr.first->second.get());
    }
    return res;
}

ResourceTexture *TResourceManager::getTexture(const char *path)
{
    return getResource<ResourceTexture>(path);
}

ResourceFont *TResourceManager::getFont(const char *path)
{
    return getResource<ResourceFont>(path);
}

ResourceMaterial *TResourceManager::getMaterial(const char *path)
{
    return getResource<ResourceMaterial>(path);
}

ResourceTexture *TResourceManager::getSkyBoxTexture(std::vector<std::string> const &faces)
{
    ResourceTexture *res{nullptr};
    // Get the parent folder
    std::filesystem::path path(faces[0]);
    auto parentFolder = path.parent_path().filename().string();

    auto itr = resources.find(parentFolder);
    if (itr != resources.end())
    {
        res = dynamic_cast<ResourceTexture *>(itr->second.get());
    }
    else
    {
        auto new_itr = resources.emplace(parentFolder, std::make_unique<ResourceTexture>());
        res = dynamic_cast<ResourceTexture *>(new_itr.first->second.get());
        res->loadTextureSkyBox(parentFolder, faces);
    }
    return res;
}

ResourceShader *TResourceManager::getShader(const char *vertPath, const char *fragPath)
{
    assert(vertPath);
    assert(fragPath);
    ResourceShader *res{nullptr};
    std::pair<std::string, std::string> shaderPair{vertPath, fragPath};
    auto itr = resources_shaders.find(shaderPair);
    if (itr != resources_shaders.end())
    {
        res = dynamic_cast<ResourceShader *>(itr->second.get());
    }
    else
    {
        auto new_itr = resources_shaders.emplace(shaderPair, std::make_unique<ResourceShader>(shaderPair.first, shaderPair.second));
        res = dynamic_cast<ResourceShader *>(new_itr.first->second.get());
    }
    return res;
}

const TResourceManager::MapShaderResources &TResourceManager::getShaders() const noexcept
{
    return resources_shaders;
}

ResourceModel *TResourceManager::getCubeResource(glm::vec3 &size)
{
    ResourceModel *res{nullptr};
    auto itr = resources_cubes.find(size);
    if (itr != resources_cubes.end())
    {
        res = dynamic_cast<ResourceModel *>(itr->second.get());
    }
    else
    {
        auto new_itr = resources_cubes.emplace(size, std::make_unique<ResourceModel>());
        res = dynamic_cast<ResourceModel *>(new_itr.first->second.get());
        res->createCube(size);
    }
    return res;
}

void TResourceManager::clearResource(const char *path)
{
    auto itr = resources.find(path);
    if (itr != resources.end())
    {
        resources.erase(itr);
    }
}