#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    explicit Vertex() = default;
    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCord) : Position(pos), Normal(norm), TexCoord(texCord) {}
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    int BoneIDs[MAX_BONE_INFLUENCE];
    float Weight[MAX_BONE_INFLUENCE];
};