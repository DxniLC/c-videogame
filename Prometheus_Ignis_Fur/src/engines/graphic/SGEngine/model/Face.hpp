#pragma once

#include <glm/vec3.hpp>
#include <vector>

struct Face
{
    explicit Face(std::vector<unsigned int> index, glm::vec3 normal, glm::vec3 center) : index(index), normal(normal), center(center) {}

    std::vector<unsigned int> index;
    glm::vec3 normal;
    glm::vec3 center;
    
};
