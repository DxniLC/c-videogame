#pragma once

#include <glm/vec2.hpp>

#include <unordered_map>

#include "Tile.hpp"

#include <engines/graphic/resource/ResourceShader.hpp>
#include <engines/graphic/resource/ResourceModel.hpp>
#include "../../scene/TNode.hpp"

struct TileBase
{
    virtual ~TileBase() = default;
};

template <typename ElementType>
struct TileManager : TileBase
{
    explicit TileManager();

    Tile<ElementType> &createTile(glm::vec2 position, glm::vec2 dimensions);
    std::vector<Tile<ElementType>*> getCurrentTiles(glm::vec2 currentPosition);

    constexpr std::vector<Tile<ElementType>> const &getTiles() const noexcept { return m_tiles; };

    constexpr void clearTiles() noexcept { m_tiles.clear(); };

private:
    std::vector<Tile<ElementType>> m_tiles;
};