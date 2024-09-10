#pragma once

#include <vector>

#include <glm/vec2.hpp>

template <typename>
struct TileManager;

template <typename ElementType>
struct Tile
{
    using InstancesMap = std::unordered_map<ResourceModel*, std::vector<glm::mat4>>;

    friend TileManager<ElementType>;

    Tile(glm::vec2 position, glm::vec2 dimension);

    void addElement(ElementType element);

    void addElementInstance(ResourceModel *model, glm::mat4 &transform);

    bool IsElementOnTile(glm::vec2 currentPosition);
    bool IsElementOnTile(glm::vec3 positionXZ);

    std::vector<ElementType> &getElements();

    InstancesMap &getInstanceElements();

    float xMin, xMax, yMin, yMax;

private:
    glm::vec2 position{};
    glm::vec2 dimension{};

    InstancesMap m_instances;

    std::vector<ElementType> m_elements;
};