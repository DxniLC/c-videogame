#pragma once

#include <engines/resource/TResource.hpp>

#include <vector>

enum TextureType
{
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
    LIGHTMAP
};

struct ResourceTexture : TResource
{
    explicit ResourceTexture() = default;
    explicit ResourceTexture(const char *filename);
    explicit ResourceTexture(std::vector<std::string> const &faces);

    void loadFile(const char *filename) override final;

    void loadTextureSkyBox(std::string const &parentFolder,std::vector<std::string> const &faces);

    unsigned int id;
    int width, height;
    std::string path;
    TextureType type;

private:
    std::string getTexturePath(std::string path);
    std::string texturesPath{"media/textures/"};
};