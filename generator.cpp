#include <random>
#include <queue>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "generator.hpp"

namespace dungeon
{
    std::ostream& operator << (std::ostream& os, const Room& r)
    {
        return os << "Room [L " << r.Left() << ", R " << r.Right() <<
                    ", T " << r.Top() << ", B " << r.Bottom() << "]";
    }


    Generator::Generator()
        : m_stage(CREATE_ROOMS),
          m_randomGen(std::random_device{}())
    {
        // Initialize the tile array - each tile needs to know its own location
        // for pathfinding.
        for (std::size_t x = 0; x < m_tiles.size(); x++) {
            for (std::size_t y = 0; y < m_tiles.size(); y++) {
                m_tiles[x][y].x = x;
                m_tiles[x][y].y = y;
            }
        }
    }


    bool Generator::FitRooms()
    {
        bool found_intersections = false;
        std::cout << "Room separation iteration " << m_fitProgress
            << std::endl;

        for (auto&& room : m_rooms) {
            unsigned int intersect_count = 0;
            int total_adjust_x = 0;
            int total_adjust_y = 0;
            int adjust_x;
            int adjust_y;

            // Check for intersections with other rooms.
            // TODO: efficient lookup for collisions with other
            // rooms, rather than scanning the whole lot - possibly
            // only worth it for large room numbers.
            for (auto&& other : m_rooms) {
                if (&room != &other &&
                    room.Intersects(other,
                                    &adjust_x,
                                    &adjust_y)) {
                    intersect_count++;
                    total_adjust_x += adjust_x;
                    total_adjust_y += adjust_y;

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
                found_intersections = true;
                int final_adjust_x =
                    (static_cast<float>(total_adjust_x) /
                     static_cast<float>(intersect_count)) * 1.1f;
                int final_adjust_y =
                    (static_cast<float>(total_adjust_y) /
                     static_cast<float>(intersect_count)) * 1.1f;
                std::cout << "Found " << intersect_count
                    << " intersections for " << room
                    << " moving (" << final_adjust_x << ", "
                    << final_adjust_y << ")" << std::endl;
                room.Move(final_adjust_x, final_adjust_y);
            }
        }

        if (!found_intersections) {
            std::cout << "Found no intersections" << std::endl;
        }

        return found_intersections;
    }


    std::vector<Tile *> Generator::GetTileNeighbours (Tile *tile)
    {
        std::vector<Tile *> neighbours;

        if (tile->x > 0) {
            neighbours.push_back(
                &m_tiles[tile->x - 1][tile->y]);
        }

        if (tile->x < MAP_WIDTH - 1) {
            neighbours.push_back(
                &m_tiles[tile->x + 1][tile->y]);
        }

        if (tile->y > 0) {
            neighbours.push_back(
                &m_tiles[tile->x][tile->y - 1]);
        }

        if (tile->y < MAP_HEIGHT - 1) {
            neighbours.push_back(
                &m_tiles[tile->x][tile->y + 1]);
        }

        return neighbours;
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
            Tile *start = &m_tiles[static_cast<int>(edge.p1.x)]
                                    [static_cast<int>(edge.p1.y)];
            Tile *end = &m_tiles[static_cast<int>(edge.p2.x)]
                                    [static_cast<int>(edge.p2.y)];

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
                    for (auto *neighbour : GetTileNeighbours(current)) {
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
                current->filled = true;
                current = came_from[current];
            }
        }
    }


    void Generator::RoomsToTiles()
    {
        // Clear the map.
        for (std::size_t x = 0; x < m_tiles.size(); x++) {
            for (std::size_t y = 0; y < m_tiles[x].size(); y++) {
                m_tiles[x][y].filled = false;
            }
        }

        // Fill in the map squares taken by rooms.
        for (auto &&room : m_rooms) {
            for (int x = room.Left(); x < room.Right(); x++) {
                for (int y = room.Top(); y < room.Bottom(); y++) {
                    m_tiles[x][y].filled = true;
                }
            }
        }
    }


    bool Generator::Iterate()
    {
        bool finished = false;

        switch(m_stage) {
        case CREATE_ROOMS:
            if (m_rooms.size() == MAP_ROOMS) {
                // We've created all the rooms we need - move onto the next
                // stage
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
            break;

        case FIT_ROOMS:
            if (m_fitProgress == SEPARATION_ITERS) {
                // We've done as many iterations as we wanted - move onto the
                // next stage
                m_stage = DISCARD_ROOMS;
            } else {
                ++m_fitProgress;

                bool intersections = FitRooms();
                if (!intersections) {
                    m_stage = DISCARD_ROOMS;
                }

                RoomsToTiles();
            }
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
            m_stage = FINISHED;
            break;

        case FINISHED:
        default:
            finished = true;
            break;
        }

        return finished;
    }


    void Generator::Draw(SDL_Renderer *renderer) const
    {
        SDL_Rect rect;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (std::size_t x = 0; x < m_tiles.size(); x++) {
            for (std::size_t y = 0; y < m_tiles[x].size(); y++) {
                if (m_tiles[x][y].filled) {
                    rect.x = x * TILE_WIDTH;
                    rect.y = y * TILE_HEIGHT;
                    rect.w = TILE_WIDTH;
                    rect.h = TILE_HEIGHT;

                    SDL_RenderFillRect(renderer, &rect);
                }
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
    }
}
