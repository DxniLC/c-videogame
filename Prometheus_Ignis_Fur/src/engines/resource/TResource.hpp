#pragma once

#include <string>

#include <glm/mat4x4.hpp>

struct ResourceShader;
struct TResourceManager;
struct TResource
{
    virtual ~TResource() = default;

    virtual void loadFile(const char *filename = nullptr) = 0;
    // WARNING_TO_FIX
    virtual void draw([[maybe_unused]] glm::mat4 matrixTransform){};

    constexpr std::string getFileName() const noexcept
    {
        return filename;
    }

    // return currentShader if had one
    [[maybe_unused]] ResourceShader *setShader(ResourceShader *shader)
    {
        ResourceShader *lastShader{nullptr};
        if (this->shader)
            lastShader = this->shader;
        this->shader = shader;
        return lastShader;
    }

    [[nodiscard]] ResourceShader *getShader() noexcept { return shader; };

    static void setResourceManager(TResourceManager &resourceMan)
    {
        resourceManager = &resourceMan;
    }

protected:
    inline static TResourceManager *resourceManager{nullptr};
    std::string filename;
    ResourceShader *shader{nullptr};
};