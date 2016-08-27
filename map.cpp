#include "map.hpp"
namespace dungeon
{
    std::vector<Tile *> Map::GetTileNeighbours (Tile *tile, bool diags)
    {
        std::vector<Tile *> neighbours;

        if (tile->x > 0) {
            neighbours.push_back(
                &m_tiles[CoordsToTileIndex(tile->x - 1, tile->y)]);
        }

        if (tile->x < MAP_WIDTH - 1) {
            neighbours.push_back(
                &m_tiles[CoordsToTileIndex(tile->x + 1, tile->y)]);
        }

        if (tile->y > 0) {
            neighbours.push_back(
                &m_tiles[CoordsToTileIndex(tile->x, tile->y - 1)]);
        }

        if (tile->y < MAP_HEIGHT - 1) {
            neighbours.push_back(
                &m_tiles[CoordsToTileIndex(tile->x, tile->y + 1)]);
        }

        if (diags) {
            if (tile->x > 0 && tile->y > 0) {
                neighbours.push_back(
                    &m_tiles[CoordsToTileIndex(tile->x - 1, tile->y - 1)]);
            }

            if (tile->x > 0 && tile->y < MAP_HEIGHT - 1) {
                neighbours.push_back(
                    &m_tiles[CoordsToTileIndex(tile->x - 1, tile->y + 1)]);
            }

            if (tile->x < MAP_WIDTH - 1 && tile->y > 0) {
                neighbours.push_back(
                    &m_tiles[CoordsToTileIndex(tile->x + 1, tile->y - 1)]);
            }

            if (tile->x < MAP_WIDTH - 1 && tile->y < MAP_WIDTH - 1) {
                neighbours.push_back(
                    &m_tiles[CoordsToTileIndex(tile->x + 1, tile->y + 1)]);
            }
        }

        return neighbours;
    }
}
