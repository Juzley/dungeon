#include <SDL2/SDL.h>
#include <array>
#include <vector>
#include <random>

#include "graph.hpp"


namespace dungeon
{
    class Room
    {
        public:
            Room(int          left,
                 int          top,
                 unsigned int width,
                 unsigned int height)
                : left(left), top(top), width(width), height(height)
            {
            }

            bool Intersects (Room &other,
                             int  *adjust_x = NULL,
                             int  *adjust_y = NULL)
            {
                if (Left() < other.Right() && Right() > other.Left() &&
                    Top() < other.Bottom() && Bottom() > other.Top()) {
                    // Intersects, find the intersection distance if required.

                    if (adjust_x != NULL) {
                        // Work out if moving left or right is shorter.
                        if (std::abs(Right() - other.Left()) >
                            std::abs(other.Right() - Left())) {
                            *adjust_x = other.Left() - Right();
                        } else {
                            *adjust_x = other.Right() - Left();
                        }
                    }
                    
                    if (adjust_y != NULL) {
                        // Work out if moving up or down is shorter.
                        if (std::abs(Bottom() - other.Top()) >
                            std::abs(other.Bottom() - Top())) {
                            *adjust_y = other.Top() - Bottom();
                        } else {
                            *adjust_y = other.Bottom() - Top();
                        }
                    }

                    return true;
                }

                return false;
            }

            void Move(int x, int y)
            {
                left += x;
                top += y;
            }

            int Left() const
            {
                return left;
            }

            int Right() const
            {
                return left + static_cast<int>(width);
            }

            int Top() const
            {
                return top;
            }

            int Bottom() const
            {
                return top + static_cast<int>(height);
            }

            unsigned int Width() const
            {
                return width;
            }
            
            unsigned int Height() const
            {
                return height;
            }

            int CenterX() const
            {
                return static_cast<int>(static_cast<float>(left) +
                                        static_cast<float>(width) / 2.0f);
            }

            int CenterY() const
            {
                return static_cast<int>(static_cast<float>(top) +
                                        static_cast<float>(height) / 2.0f);
            }

        private:
            int          left;
            int          top;
            unsigned int width;
            unsigned int height;
    };


    struct Tile
    {
        public:
            enum TileType {
                FLOOR,
                WALL,
                EMPTY
            };

            Tile() : type(EMPTY)
            {
            }

            unsigned int x;
            unsigned int y;
            TileType     type;
    };


    class Generator
    {
        public:
            Generator();

            bool Iterate();
            void Draw(SDL_Renderer *renderer) const;

        private:
            static const unsigned int MAP_ROOMS = 20;
            static const unsigned int SEPARATION_ITERS = 20;
            static const unsigned int ROOM_SIZE_MIN = 8;
            static const unsigned int ROOM_SIZE_MAX = 32;
            static const unsigned int MAP_WIDTH = 160;
            static const unsigned int MAP_HEIGHT = 120;
            static const unsigned int TILE_WIDTH = 5;
            static const unsigned int TILE_HEIGHT = 5;

            enum GeneratorStage {
                CREATE_ROOMS,
                FIT_ROOMS,
                DISCARD_ROOMS,
                CONNECT_ROOMS,
                CREATE_PATHS,
                CREATE_WALLS,
                FINISHED
            };

            bool FitRooms();
            void CreatePaths();
            void CreateWalls();
            std::vector<Tile *> GetTileNeighbours(Tile *tile, bool diags = true);
            int PathHeuristic (Tile *tile, Tile *goal);
            bool RoomOutOfBounds(Room &room, int *adjust_x = NULL, int *adjust_y = NULL);
            void RoomsToTiles();

            GeneratorStage               m_stage;
            std::mt19937                 m_randomGen;
            unsigned int                 m_fitProgress;
            std::vector<Room>            m_rooms;
            std::vector<graph::Edge>     m_urquhartEdges;
            std::vector<graph::Triangle> m_delaunayTris;
            std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> m_tiles;
    };
}
