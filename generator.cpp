#include <SDL2/SDL.h>
#include <cmath>
#include <random>
#include <queue>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "generator.hpp"
#include "game.hpp"
#include "util.hpp"

namespace dungeon
{
    std::ostream& operator << (std::ostream& os, const Room& r)
    {
        return os << "Room [L " << r.Left() << ", R " << r.Right() <<
                    ", T " << r.Top() << ", B " << r.Bottom() << "]";
    }


    void Generator::FitRoom()
    {
        unsigned int intersect_count = 0;
        int total_adjust_x = 0;
        int total_adjust_y = 0;
        int adjust_x;
        int adjust_y;

        // Check for intersections with other rooms.
        // TODO: efficient lookup for collisions with other
        // rooms, rather than scanning the whole lot - possibly
        // only worth it for large room numbers.
        Room &room = *m_currentRoom;
        for (auto&& other : m_rooms) {
            if (&room != &other &&
                room.Intersects(other,
                                &adjust_x,
                                &adjust_y)) {
                m_collideRooms.push_back(other);
                intersect_count++;

                if (std::abs(adjust_x) < std::abs(adjust_y)) {
                    total_adjust_x += adjust_x;
                } else {
                    total_adjust_y += adjust_y;
                }

                std::cout << room << " intersects with "
                    << other << ", adjust (" << adjust_x
                    << ", " << adjust_y << ")" << std::endl;
            }

        }

        // Check for intersections with walls.
        if (RoomOutOfBounds(room, &adjust_x, &adjust_y)) {
            intersect_count++;
            total_adjust_x += adjust_x;
            total_adjust_y += adjust_y;
            std::cout
                << room << " intersects with walls, adjust ("
                << adjust_x << ", " << adjust_y << ")"
                << std::endl;
        }

        if (intersect_count > 0) {
            m_foundIntersection = true;
            int final_adjust_x =
                util::symmetric_ceil(
                    (static_cast<float>(total_adjust_x) /
                     static_cast<float>(intersect_count)) * 1.2f);
            int final_adjust_y =
                util::symmetric_ceil(
                    (static_cast<float>(total_adjust_y) /
                     static_cast<float>(intersect_count)) * 1.2f);
            std::cout << "Found " << intersect_count
                << " intersections for " << room
                << " moving (" << final_adjust_x << ", "
                << final_adjust_y << ")" << std::endl;
            m_lastMove.first = room;
            room.Move(final_adjust_x, final_adjust_y);
            m_lastMove.second = room;
        }
    }


    int Generator::PathHeuristic (Tile *tile, Tile *goal)
    {
        int x_diff = static_cast<int>(tile->x) -
                                        static_cast<int>(goal->x);
        int y_diff = static_cast<int>(tile->y) -
                                        static_cast<int>(goal->y);
        return x_diff * x_diff + y_diff * y_diff;
    }


    bool Generator::RoomOutOfBounds (Room &room, int  *adjust_x, int  *adjust_y)
    {
        bool out = false;

        if (adjust_x != NULL) {
            *adjust_x = 0;
        }

        if (adjust_y != NULL) {
            *adjust_y = 0;
        }
        
        if (room.Left() < 0) {
            out = true;
            if (adjust_x != NULL) {
                *adjust_x = -room.Left();
            }
        } else if (room.Right() > static_cast<int>(MAP_WIDTH) - 1) {
            out = true;
            if (adjust_x != NULL) {
                *adjust_x =
                    static_cast<int>(MAP_WIDTH) - room.Right() - 1;
            }
        }

        if (room.Top() < 0) {
            out = true;
            if (adjust_y != NULL) {
                *adjust_y = -room.Top();
            }
        } else if (room.Bottom() > static_cast<int>(MAP_HEIGHT) - 1) {
            out = true;

            if (adjust_y != NULL) {
                *adjust_y =
                    static_cast<int>(MAP_HEIGHT) - room.Bottom() - 1;
            }
        }

        return out;
    }


    void Generator::CreatePaths()
    {
        // Use A* to plot paths between the rooms.
        for (auto &&edge : m_urquhartEdges) {
            Tile *start = &m_map->GetTile(
                                    static_cast<int>(edge.p1.x),
                                    static_cast<int>(edge.p1.y));
            Tile *end = &m_map->GetTile(
                                    static_cast<int>(edge.p2.x),
                                    static_cast<int>(edge.p2.y));

            std::unordered_map<Tile *, int> cost_so_far;
            std::unordered_map<Tile *, Tile *> came_from;
            std::priority_queue<
                std::pair<int, Tile *>,
                std::vector<std::pair<int, Tile *>>,
                std::greater<std::pair<int, Tile *>>> open;
            std::vector<Tile *> closed;

            came_from[start] = start;
            cost_so_far[start] = 0;
            open.emplace(0, start);

            while (!open.empty()) {
                auto current = open.top().second;
                open.pop();

                if (current == end) {
                    break;
                } else {
                    // Don't consider diagonal neighbours when creating paths
                    // so that diagonal paths are wider.
                    for (auto *neighbour : m_map->GetTileNeighbours(current, false)) {
                        int new_cost = cost_so_far[current] + 1;

                        if (!cost_so_far.count(neighbour) ||
                                new_cost < cost_so_far[neighbour]) {
                            cost_so_far[neighbour] = new_cost;

                            open.emplace(PathHeuristic(neighbour, end),
                                         neighbour);
                            came_from[neighbour] = current;
                        }
                    }
                }
            }

            // Mark the path on the map.
            auto current = came_from[end];
            while (current != start) {
                current->type = Tile::FLOOR;
                current = came_from[current];
            }
        }
    }


    void Generator::CreateWalls()
    {
        for (auto iter = m_map->beginTiles(); iter != m_map->endTiles(); ++iter)  {
            if (iter->type == Tile::FLOOR) {
                for (auto *tile : m_map->GetTileNeighbours(iter)) {
                    if (tile->type == Tile::EMPTY) {
                        tile->type = Tile::WALL;
                    }
                }

                // If the tile is at the edge of the map, make it a wall.
                if (iter->x == 0 || iter->x == MAP_WIDTH - 1 ||
                    iter->y == 0 || iter->y == MAP_HEIGHT - 1) {
                    iter->type = Tile::WALL;
                }
            }
        }
    }


    void Generator::PlacePlayer()
    {
        // Pick a non-empty tile to spawn the player.
        std::uniform_int_distribution<unsigned int> tile_dis(
            0, std::count_if(
                    m_map->beginTiles(), m_map->endTiles(),
                    [](const Tile& tile) { return tile.type == Tile::FLOOR; }));
        unsigned int tileIdx = tile_dis(m_randomGen);
        for (auto tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            if (tileIter->type == Tile::FLOOR) {
                if (--tileIdx == 0) {
                    tileIter->spawn = true;
                }
            }
        }
    }


    void Generator::PlaceDoors()
    {
        // Note that here we use EMPTY to mean 'any tile allowed', rather than
        // that the tile has to be empty.
        const Tile::TileType door_patterns[4][3][3] = {
            {
                { Tile::EMPTY, Tile::FLOOR, Tile::EMPTY },
                { Tile::WALL, Tile::FLOOR, Tile::WALL },
                { Tile::FLOOR, Tile::FLOOR, Tile::FLOOR }
            },
            {
                { Tile::FLOOR, Tile::WALL, Tile::EMPTY },
                { Tile::FLOOR, Tile::FLOOR, Tile::FLOOR },
                { Tile::FLOOR, Tile::WALL, Tile::EMPTY },
            },
            {
                { Tile::FLOOR, Tile::Tile::FLOOR, Tile::FLOOR },
                { Tile::WALL, Tile::FLOOR, Tile::WALL },
                { Tile::EMPTY, Tile::FLOOR, Tile::EMPTY },
            },
            {
                { Tile::EMPTY, Tile::WALL, Tile::FLOOR },
                { Tile::FLOOR, Tile::FLOOR, Tile::FLOOR },
                { Tile::EMPTY, Tile::WALL, Tile::FLOOR }
            }
        };

        for (auto tileIter = m_map->beginTiles(); tileIter != m_map->endTiles(); ++tileIter) {
            if (tileIter->x == 0 || tileIter->y == 0 ||
                tileIter->x == m_map->Width() - 1 || tileIter->y == m_map->Height() - 1) {
                // No doors at the edge of the map.
                continue;
            }

            // Search for the 'door pattern' - a set of tiles suitable for placing
            // a door.
            bool matches_pattern = false;
            for (size_t p = 0; !matches_pattern && p < 4; p++) {
                matches_pattern = true;
                for (unsigned int x = 0; x <= 2; x++) {
                    for (unsigned int y = 0; y <= 2; y++) {
                        auto type = m_map->GetTile(tileIter->x + x - 1,
                                                   tileIter->y + y - 1).type;
                        if (door_patterns[p][y][x] != Tile::EMPTY &&
                            door_patterns[p][y][x] != type) {
                            matches_pattern = false;
                        }
                    }
                }
            }

            if (matches_pattern) {
                tileIter->type = Tile::DOOR_CLOSED;
            }
        }
    }


    void Generator::RoomsToTiles()
    {
        // Clear the map.
        m_map->Clear();

        // Fill in the map squares taken by rooms.
        for (auto&& room : m_rooms) {
            for (int x = room.Left(); x < room.Right(); x++) {
                for (int y = room.Top(); y < room.Bottom(); y++) {
                    // Ignore squares outside the map - the rooms will all be
                    // fitted inside the map later.
                    if (x > 0 && y > 0 &&
                        x < static_cast<int>(m_map->Width()) &&
                        y < static_cast<int>(m_map->Height())) {
                        m_map->GetTile(x, y).type = Tile::FLOOR;
                    }
                }
            }
        }
    }


    void Generator::Iterate()
    {
        std::cout << "Generator Iteration: Stage " <<
            Generator::StageToString(m_stage) << std::endl;
        switch(m_stage) {
        case CREATE_ROOMS:
            do {
                if (m_rooms.size() == MAP_ROOMS) {
                    // We've created all the rooms we need - move onto the next
                    // stage
                    m_currentRoom = m_rooms.begin();
                    m_foundIntersection = false;
                    m_collideRooms.clear();
                    m_stage = FIT_ROOMS;
                } else {
                    // Create another room
                    std::uniform_int_distribution<> x_dis(0, MAP_WIDTH);
                    std::uniform_int_distribution<> y_dis(0, MAP_HEIGHT);
                    std::uniform_int_distribution<> size_dis(ROOM_SIZE_MIN,
                                                             ROOM_SIZE_MAX);

                    Room room(x_dis(m_randomGen),
                              y_dis(m_randomGen),
                              size_dis(m_randomGen),
                              size_dis(m_randomGen));
                    m_rooms.push_back(room);
                    RoomsToTiles();
                }
            } while (m_stage == CREATE_ROOMS);
            break;

        case FIT_ROOMS:
            do {
                if (m_currentRoom == m_rooms.end()) {
                    // Finished the current iteration, see if we need to
                    // continue
                    m_fitProgress++;
                    if (!m_foundIntersection ||
                        m_fitProgress >= SEPARATION_ITERS) {
                        m_stage = DISCARD_ROOMS;
                    } else {
                        m_foundIntersection = false;
                        m_currentRoom = m_rooms.begin();
                        std::cout << "Room separation iteration " << m_fitProgress
                            << std::endl;
                    }
                } else {
                    // Move to the next room in the current iteration.
                    m_collideRooms.clear();
                    FitRoom();
                    ++m_currentRoom;
                }

                RoomsToTiles();
            } while (m_stage == FIT_ROOMS);
            break;

        case DISCARD_ROOMS:
        {
            // Discard any rooms that are out of bounds or intersecting
            // other rooms. Remove out-of-bounds rooms first, to ensure
            // that the minimal number of rooms are removed in the case
            // where there are rooms that are out-of-bounds and also
            // intersecting other rooms.
            m_rooms.erase(std::remove_if(
                            m_rooms.begin(), m_rooms.end(),
                            [this](Room r) { return RoomOutOfBounds(r); }),
                          m_rooms.end());

            auto roomIter = m_rooms.begin();
            while (roomIter != m_rooms.end()) {
                bool erased = false;
                auto &room = *roomIter;

                for (auto&& other : m_rooms) {
                    if (&room != &other && room.Intersects(other)) {
                        roomIter = m_rooms.erase(roomIter);
                        erased = true;
                    }
                }

                if (!erased) {
                    ++roomIter;
                }
            }
            RoomsToTiles();
            m_stage = CONNECT_ROOMS;

            break;
        }
        case CONNECT_ROOMS:
        {
            using namespace graph;
            std::vector<Vec2f> centers;
            for(auto &&room : m_rooms) {
                centers.push_back(Vec2f(room.CenterX(), room.CenterY()));
            }

            // Generate the delaunay graph just so we can display it, we're
            // not going to actually use it in the pathfinding.
            m_delaunayTris = generateDelaunay(centers);

            // Use an Urquhart graph to determine the connections between
            // the rooms.
            m_urquhartEdges = generateUrquhart(centers);
            m_stage = CREATE_PATHS;
            break;
        }
        case CREATE_PATHS:
            CreatePaths();
            m_stage = CREATE_WALLS;
            break;
            
        case CREATE_WALLS:
            CreateWalls();
            m_stage = PLACE_PLAYER;
            break;

        case PLACE_PLAYER:
            PlacePlayer();
            m_stage = PLACE_DOORS;
            break;

        case PLACE_DOORS:
            PlaceDoors();
            m_stage = FINISHED;
            break;

        case FINISHED:
        default:
            break;
        }
    }


    void Generator::DrawRoomOutline(SDL_Renderer *renderer,
                                    const Room   &room) const
    {
        // Top
        SDL_RenderDrawLine(renderer,
                           room.Left() * TILE_WIDTH,
                           room.Top() * TILE_HEIGHT,
                           room.Right() * TILE_WIDTH,
                           room.Top() * TILE_HEIGHT);
        // Right
        SDL_RenderDrawLine(renderer,
                           room.Right() * TILE_WIDTH,
                           room.Top() * TILE_HEIGHT,
                           room.Right() * TILE_WIDTH,
                           room.Bottom() * TILE_HEIGHT);

        // Bottom
        SDL_RenderDrawLine(renderer,
                           room.Right() * TILE_WIDTH,
                           room.Bottom() * TILE_HEIGHT,
                           room.Left() * TILE_WIDTH,
                           room.Bottom() * TILE_HEIGHT);

        // Left
        SDL_RenderDrawLine(renderer,
                           room.Left() * TILE_WIDTH,
                           room.Bottom() * TILE_HEIGHT,
                           room.Left() * TILE_WIDTH,
                           room.Top() * TILE_HEIGHT);
    }


    void Generator::Draw(SDL_Renderer *renderer) const
    {
        SDL_Rect rect;
        for (auto iter = m_map->beginTiles();
             iter != m_map->endTiles();
             ++iter) {
            const Tile &tile = *iter;
            if (!tile.IsEmpty()) {
                if (tile.type == Tile::FLOOR) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else if (tile.type == Tile::WALL) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                }

                rect.x = tile.x * TILE_WIDTH;
                rect.y = tile.y * TILE_HEIGHT;
                rect.w = TILE_WIDTH;
                rect.h = TILE_HEIGHT;
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto &&tri : m_delaunayTris) {
            for (auto &&edge : tri.GetEdges()) {
                SDL_RenderDrawLine(renderer,
                                   edge.p1.x * TILE_WIDTH,
                                   edge.p1.y * TILE_HEIGHT,
                                   edge.p2.x * TILE_WIDTH,
                                   edge.p2.y * TILE_HEIGHT);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (auto &&edge : m_urquhartEdges) {
            SDL_RenderDrawLine(renderer,
                               edge.p1.x * TILE_WIDTH,
                               edge.p1.y * TILE_HEIGHT,
                               edge.p2.x * TILE_WIDTH,
                               edge.p2.y * TILE_HEIGHT);
        }

        if (m_stage == FIT_ROOMS && m_foundIntersection) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            for (auto &&room : m_collideRooms) {
                DrawRoomOutline(renderer, room);
            }

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            DrawRoomOutline(renderer, m_lastMove.first);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            DrawRoomOutline(renderer, m_lastMove.second);

            int start_x = (m_lastMove.first.Left() + m_lastMove.first.Width() / 2);
            int start_y = (m_lastMove.first.Top() + m_lastMove.first.Height() / 2);
            int end_x = (m_lastMove.second.Left() + m_lastMove.second.Width() / 2);
            int end_y = (m_lastMove.second.Top() + m_lastMove.second.Height() / 2);
            SDL_RenderDrawLine(renderer,
                               start_x * TILE_WIDTH,
                               start_y * TILE_HEIGHT,
                               end_x * TILE_WIDTH,
                               end_y * TILE_HEIGHT);
        }
    }


    void GeneratorGameState::Run()
    {
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                m_generator.Iterate();
                if (m_generator.IsFinished()) {
                    // Switch to a new game gamestate
                    m_manager.Replace(std::make_shared<Game>(m_manager, m_generator.GetMap()));
                }
            }
        }
    }
}
