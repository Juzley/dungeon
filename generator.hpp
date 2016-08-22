#include <SDL2/SDL.h>
#include <vector>
#include <random>

#include "graph.hpp"


namespace dungeon
{
    class Room;

    class Generator
    {
        public:
            void Generator();

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
                FINISHED
            };

            void FitRooms();
            void CreatePaths();
            std::vector<Tile *> GetTileNeighbours(Tile *tile);
            int PathHeuristic (Tile *tile, Tile *goal);
            bool RoomOutOfBounds(Room &room, int *adjust_x = NULL, int *adjust_y = NULL);
            void Generator::RoomsToTiles();

            GeneratorStage              m_stage;
            std::mt19937                m_randomGen;
            unsigned int                m_fitProgress;
            std::vector<Room>           m_rooms;
            std::vector<graph::Edge>    m_urquhartEdges;
            std::vector<graph::Triangle m_delaunaryTris;
            std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> m_tiles;
    };
}
