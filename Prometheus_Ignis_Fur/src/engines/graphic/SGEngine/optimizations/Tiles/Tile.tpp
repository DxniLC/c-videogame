#include "Tile.hpp"

template <typename ElementType>
Tile<ElementType>::Tile(glm::vec2 position, glm::vec2 dimension)
    : position{position}, dimension{dimension}
{
    xMax = position.x + (dimension.x / 2.0f);
    xMin = position.x - (dimension.x / 2.0f);
    yMax = position.y + (dimension.y / 2.0f);
    yMin = position.y - (dimension.y / 2.0f);
}

template <typename ElementType>
bool Tile<ElementType>::IsElementOnTile(glm::vec3 currentPosition)
{
    return IsElementOnTile(glm::vec2(currentPosition.x, currentPosition.z));
}

template <typename ElementType>
bool Tile<ElementType>::IsElementOnTile(glm::vec2 positionXZ)
{
    if (positionXZ.x <= xMax && positionXZ.x >= xMin &&
        positionXZ.y <= yMax && positionXZ.y >= yMin)
    {
        return true;
    }
    return false;
}

template <typename ElementType>
void Tile<ElementType>::addElement(ElementType element)
{
    m_elements.emplace_back(element);
}

template <typename ElementType>
void Tile<ElementType>::addElementInstance(ResourceModel *model, glm::mat4 &transform)
{
    auto itr = m_instances.find(model);
    if (itr != m_instances.end())
    {
        itr->second.emplace_back(transform);
    }
    else
    {
        m_instances.emplace(model, std::vector<glm::mat4>{transform});
    }
}

template <typename ElementType>
std::vector<ElementType> &Tile<ElementType>::getElements()
{
    return m_elements;
}

template <typename ElementType>
typename Tile<ElementType>::InstancesMap &Tile<ElementType>::getInstanceElements()
{
    return m_instances;
}
