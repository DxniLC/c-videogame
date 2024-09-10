
#pragma once

#include <engines/resource/TResource.hpp>

struct ResourceTexture;
struct ResourceMaterial : TResource
{
    explicit ResourceMaterial() = default;

    void loadFile([[maybe_unused]] const char *filename) override final{};

    bool IsReflective{false};
    glm::vec4 diffuse{1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 specular{0.5f, 0.5f, 0.5f, 1.0f};
    float shininess{32.0f};

    ResourceTexture *diffuseMap{nullptr};
    ResourceTexture *specularMap{nullptr};
    ResourceTexture *lightMap{nullptr};

};