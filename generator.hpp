#ifndef __GENERATOR_HPP__
#define __GENERATOR_HPP__


#include <SDL2/SDL.h>
#include <array>
#include <vector>
#include <random>
#include <memory>

#include "graph.hpp"
#include "map.hpp"
#include "gamestate.hpp"


namespace dungeon
{
    class Room
    {
        public:
            Room()
            {
            }

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
                        if (std::abs(Right() - other.Left()) <
                            std::abs(other.Right() - Left())) {
                            *adjust_x = other.Left() - Right();
                        } else {
                            *adjust_x = other.Right() - Left();
                        }
                    }
                    
                    if (adjust_y != NULL) {
                        // Work out if moving up or down is shorter.
                        if (std::abs(Bottom() - other.Top()) <
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


    class Generator
    {
        public:
            Generator()
                : m_map(std::make_shared<Map>()),
                  m_stage(CREATE_ROOMS),
                  m_randomGen(std::random_device{}()),
                  m_lastMove(Room(), Room())
            {
            };

            std::shared_ptr<Map> GetMap()
            {
                return m_map;
            }

            bool IsFinished() const {
                return m_stage == FINISHED;
            }

            void Iterate();
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
                PLACE_PLAYER,
                PLACE_DOORS,
                FINISHED
            };

            static const char * StageToString(GeneratorStage stage)
            {
                switch (stage) {
                case CREATE_ROOMS:
                    return "Create Rooms";
                case FIT_ROOMS:
                    return "Fit Rooms";
                case DISCARD_ROOMS:
                    return "Discard Rooms";
                case CONNECT_ROOMS:
                    return "Connect Rooms";
                case CREATE_PATHS:
                    return "Create paths";
                case CREATE_WALLS:
                    return "Create walls";
                case PLACE_PLAYER:
                    return "Place player";
                case PLACE_DOORS:
                    return "Place doors";
                case FINISHED:
                    return "Finished";
                default:
                    return "Unknown";
                }
            }

            void FitRoom();
            void DrawRoomOutline(SDL_Renderer *renderer, const Room &room) const;
            void CreatePaths();
            void CreateWalls();
            void PlacePlayer();
            void PlaceDoors();
            int PathHeuristic (Tile *tile, Tile *goal);
            bool RoomOutOfBounds(Room &room, int *adjust_x = NULL, int *adjust_y = NULL);
            void RoomsToTiles();

            std::shared_ptr<Map>         m_map;
            GeneratorStage               m_stage;
            std::mt19937                 m_randomGen;
            unsigned int                 m_fitProgress;
            std::vector<Room>            m_rooms;
            std::vector<Room>::iterator  m_currentRoom;
            std::vector<Room>            m_collideRooms;
            std::pair<Room, Room>        m_lastMove;
            bool                         m_foundIntersection;
            std::vector<graph::Edge>     m_urquhartEdges;
            std::vector<graph::Triangle> m_delaunayTris;
    };


    class GeneratorGameState : public GameState
    {
        public:
            GeneratorGameState(GameStateManager &manager)
                : m_manager(manager)
            {
            }

            void Draw(SDL_Renderer *renderer) const override
            {
                m_generator.Draw(renderer);
            }

            void Run() override;

        private:
            SDL_Renderer     *m_renderer;
            GameStateManager &m_manager;
            Generator         m_generator;
    };
}


#endif
