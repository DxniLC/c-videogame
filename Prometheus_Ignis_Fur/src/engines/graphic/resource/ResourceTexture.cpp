#include "ResourceTexture.hpp"

#include <GLAD/src/glad.h>
#include <stb/src/stb_image.h>
#include <iostream>

#include <filesystem>

ResourceTexture::ResourceTexture(const char *filename)
{
    loadFile(filename);
}

void ResourceTexture::loadFile(const char *filename)
{
    assert(filename);
    path = getTexturePath(std::string(filename));

    // LOAD IMAGE
    glGenTextures(1, &id);

    int nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format{GL_RGBA};
        switch (nrChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GLint(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cerr << "\n*** ERROR: Failed to load texture " << path;
    }

    stbi_image_free(data);
}

std::string ResourceTexture::getTexturePath(std::string path)
{
    std::filesystem::path directory(path);
    std::filesystem::path result("media");

    bool founded{false};
    for (auto &dirs : directory)
    {
        if (dirs.string() == "textures")
        {
            founded = true;
        }
        if (founded)
        {
            result /= dirs;
        }
    }
    if (!founded)
        result /= "textures/" / directory;

    return result.string();
}

// Order: RIGHT / LEFT / TOP / BOTTOM / FRONT / BACK
void ResourceTexture::loadTextureSkyBox(std::string const &parentFolder, std::vector<std::string> const &faces)
{
    path = "SkyBox/" + parentFolder;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        GLenum format{GL_RGBA};
        switch (nrChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GLint(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;

        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}