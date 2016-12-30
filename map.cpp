#include "map.hpp"
namespace dungeon
{
    static int sign (int val)
    {
        if (val > 0) {
            return 1;
        } else if (val < 0) {
            return -1;
        } else {
            return 0;
        }
    }


    bool Map::CheckCornerVisibility(unsigned int cornerx, unsigned int cornery,
                                    int signx, int signy) const
    {
        if (GetTile(cornerx, cornery).BlocksVisibility()) {
            if (GetTile(cornerx - 1, cornery).BlocksVisibility()) {
                return false;
            }
            if (GetTile(cornerx, cornery - 1).BlocksVisibility()) {
                return false;
            }
        }

        if (GetTile(cornerx - 1, cornery - 1).BlocksVisibility()) {
            if (GetTile(cornerx, cornery - 1).BlocksVisibility()) {
                return false;
            }
            if (GetTile(cornerx - 1, cornery).BlocksVisibility()) {
                return false;
            }
        }

        if (signx * signy == 1) {
            if (GetTile(cornerx, cornery).BlocksVisibility() ||
                GetTile(cornerx - 1, cornery - 1).BlocksVisibility()) {
                return false;
            }
        } else if (signx * signy == -1) {
            if (GetTile(cornerx - 1, cornery).BlocksVisibility() ||
                GetTile(cornerx, cornery - 1).BlocksVisibility()) {
                return false;
            }
        } else {
            if (GetTile(cornerx - 1, cornery - 1).BlocksVisibility() ||
                GetTile(cornerx - 1, cornery).BlocksVisibility() ||
                GetTile(cornerx, cornery - 1).BlocksVisibility() ||
                GetTile(cornerx, cornery).BlocksVisibility()) {
                return false;
            }
        }

        return true;
    }

    bool Map::CheckRayVisibility(unsigned int startx, unsigned int starty,
                                 unsigned int endx, unsigned int endy) const
    {
        int signx = sign(static_cast<int>(endx) - static_cast<int>(startx));
        int signy = sign(static_cast<int>(endy) - static_cast<int>(starty));

        // Check whether the immediate neighbour in the given direction blocks.
        if (signx == 0 && signy == -1 &&
            GetTile(startx, starty - 1).BlocksVisibility() &&
            GetTile(startx - 1, starty - 1).BlocksVisibility()) {
            return false;
        }
        if (signx == -1 && signy == 0 &&
            GetTile(startx - 1, starty).BlocksVisibility() &&
            GetTile(startx - 1, starty - 1).BlocksVisibility()) {
            return false;
        }
        if (signx == 0 && signy == 1 &&
            GetTile(startx, starty).BlocksVisibility() &&
            GetTile(startx - 1, starty).BlocksVisibility()) {
            return false;
        }
        if (signx == 1 && signy == 0 &&
            GetTile(startx, starty).BlocksVisibility() &&
            GetTile(startx, starty - 1).BlocksVisibility()) {
            return false;
        }
        if (signx == 1 && signy == 1 &&
            GetTile(startx, starty).BlocksVisibility()) {
            return false;
        }
        if (signx == -1 && signy == 1 &&
            GetTile(startx - 1, starty).BlocksVisibility()) {
            return false;
        }
        if (signx == -1 && signy == -1 &&
            GetTile(startx - 1, starty - 1).BlocksVisibility()) {
            return false;
        }
        if (signx == 1 && signy == -1 &&
            GetTile(startx, starty - 1).BlocksVisibility()) {
            return false;
        }

        unsigned int dx = std::abs(static_cast<int>(endx) -
                                   static_cast<int>(startx));
        unsigned int dy = std::abs(static_cast<int>(endy) -
                                   static_cast<int>(starty));

        int ny;
        int nx;
        int ry;
        int rx;
        unsigned int cellx;
        unsigned int celly;

        // Check vertical intersections
        if (dx != 0) {
            for (int x = 1; x < static_cast<int>(dx); x++) {
                ny = x * dy / dx;
                ry = x * dy % dx;

                cellx = static_cast<int>(startx) + x * signx;
                celly = static_cast<int>(starty) + ny * signy;

                if (ry != 0) {
                    if (signy < 0) {
                        celly--;
                    }

                    // Check for a wall between this tile and the tile to the
                    // left (i.e. if either this tile or the tile to the left
                    // blocks vis).
                    if (GetTile(cellx, celly).BlocksVisibility() ||
                        GetTile(cellx - 1, celly).BlocksVisibility()) {
                        return false;
                    }
                } else {
                    // The ray intersects with a corner, need special
                    // handling.
                    if (!CheckCornerVisibility(cellx, celly, signx, signy)) {
                        return false;
                    }
                }
            }
        }

        // Check horizontal intersections
        if (dy != 0) {
            for (int y = 1; y < static_cast<int>(dy); y++) {
                nx = y * dx / dy;
                rx = y * dx % dy;

                cellx = static_cast<int>(startx) + nx * signx;
                celly = static_cast<int>(starty) + y * signy;

                if (rx != 0) {
                    if (signx < 0) {
                        cellx--;
                    }

                    // Check for a wall between this tile and the tile above it
                    // (i.e. if either this tile or the tile above it blocks
                    // vis).
                    if (GetTile(cellx, celly).BlocksVisibility() ||
                        GetTile(cellx, celly - 1).BlocksVisibility()) {
                        return false;
                    }
                } else {
                    // The ray intersects with a corner, need special
                    // handling.
                    if (!CheckCornerVisibility(cellx, celly, signx, signy)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool Map::IsVisible (unsigned int startx, unsigned int starty,
                         unsigned int endx, unsigned int endy) const
    {
        unsigned int corners[4][2] = {
            { 0, 0 }, // Top left
            { 1, 0 }, // Top right
            { 1, 1 }, // Bottom right
            { 0, 1 }, // Bottom left
        };

        for (size_t i = 0; i < 4; i++) {
            for (size_t j = 0; j < 4; j++) {
                bool vis = CheckRayVisibility(startx + corners[i][0],
                                              starty + corners[i][1],
                                              endx + corners[j][0],
                                              endy + corners[j][1]);
                if (vis) {
                    return true;
                }
            }
        }

        return false;
    }


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
