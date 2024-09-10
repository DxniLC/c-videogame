
#include "TileManager.hpp"

template <typename ElementType>
TileManager<ElementType>::TileManager()
{
    m_tiles.reserve(50);
}
template <typename ElementType>
Tile<ElementType> &TileManager<ElementType>::createTile(glm::vec2 position, glm::vec2 dimensions)
{
    return m_tiles.emplace_back(position, dimensions);
}
template <typename ElementType>
std::vector<Tile<ElementType> *> TileManager<ElementType>::getCurrentTiles(glm::vec2 currentPosition)
{
    std::vector<Tile<ElementType> *> currentTiles;
    for (auto &tile : m_tiles)
    {
        if (currentPosition.x <= tile.xMax && currentPosition.x >= tile.xMin &&
            currentPosition.y <= tile.yMax && currentPosition.y >= tile.yMin)
        {
            currentTiles.emplace_back(&tile);
        }
    }
    return currentTiles;
}
