#include "TMesh.hpp"

#include <engines/graphic/resource/ResourceModel.hpp>

#include <glm/mat4x4.hpp>

void TMesh::update()
{
    mesh->draw(matrix);
}

void TMesh::setTexture(ResourceTexture *texture)
{
    mesh->setTexture(texture);
}
