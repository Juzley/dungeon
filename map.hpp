#ifndef __MAP_HPP__
#define __MAP_HPP__


#include <array>
#include <vector>
#include <algorithm>


namespace dungeon
{
    struct Tile
    {
        public:
            enum TileType {
                FLOOR,
                WALL,
                STAIRS,
                EMPTY
            };

            Tile() : type(EMPTY)
            {
            }

            bool IsEmpty() const
            {
                return type == EMPTY;
            }

            unsigned int x;
            unsigned int y;
            TileType     type;
            bool         spawn;
            bool         seen;
    };

    
    class Map
    {
        public:
            static const unsigned int MAP_HEIGHT = 120;
            static const unsigned int MAP_WIDTH = 160;

            using TileArray = std::array<Tile, Map::MAP_WIDTH * Map::MAP_HEIGHT>;
            using TileIter = TileArray::iterator;
            using ConstTileIter = TileArray::const_iterator;

            Map()
            {
                // Initialize the tile array. Tiles know their own location,
                // for pathfinding.
                for (TileArray::size_type i = 0; i < m_tiles.size(); i++) {
                    m_tiles[i].x = i % MAP_WIDTH;
                    m_tiles[i].y = i / MAP_WIDTH;
                }
            }

            Tile& GetTile(unsigned int x, unsigned int y)
            {
                return m_tiles[CoordsToTileIndex(x, y)];
            }

            std::vector<Tile *> GetTileNeighbours (Tile *tile,
                                                   bool  diags = true);

            TileIter beginTiles() { return m_tiles.begin(); }
            TileIter endTiles() { return m_tiles.end(); }
            ConstTileIter beginTiles() const { return m_tiles.begin(); }
            ConstTileIter endTiles() const { return m_tiles.end(); }
            ConstTileIter cbeginTiles() const { return m_tiles.cbegin(); }
            ConstTileIter cendTiles() const { return m_tiles.cend(); }

            void Clear()
            {
                for (auto &&tile : m_tiles) {
                    tile.type = Tile::EMPTY;
                }
            }

            unsigned int Width() const { return MAP_WIDTH; }
            unsigned int Height() const { return MAP_HEIGHT; }

        private:
            TileArray m_tiles;

            std::size_t CoordsToTileIndex(unsigned int x,
                                          unsigned int y)
            {
                return (y * MAP_WIDTH + x);
            }
    };
}


#endif
