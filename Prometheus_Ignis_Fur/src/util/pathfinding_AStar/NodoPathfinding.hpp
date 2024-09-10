#pragma once

#include <glm/glm.hpp>
#include <memory>

struct NodoPathfinding
{
    NodoPathfinding *padre{nullptr};
    glm::vec2 posicion{};
    float f{0};
    float g{0};
    float h{0};

    NodoPathfinding(NodoPathfinding *padre, glm::vec2 posicion) : padre{padre}, posicion{posicion}{}

    bool isNone()
    {
        if (this != NULL)
        {
            return true;
        }
        return false;
    }
};