#include <SDL2/SDL.h>
#include <array>
#include <random>
#include <cmath>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "graph.hpp"


namespace dungeon
{
    struct Point
    {
        int x;
        int y;
    };


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

    std::ostream& operator << (std::ostream& os, const Room& r)
    {
        return os << "Room [L " << r.Left() << ", R " << r.Right() <<
                    ", T " << r.Top() << ", B " << r.Bottom() << "]";
    }


    struct Tile
    {
        public:
            Tile() : filled(false)
            {
            }

            unsigned int x;
            unsigned int y;
            bool         filled;
    };


    class Map
    {
        public:
            Map ()
            {
                std::vector<Room> rooms;
                std::random_device rd;
                std::mt19937 gen(rd());

                // Hack, -6 to avoid generating rooms off the map.
                std::uniform_int_distribution<> x_dis(0, MAP_WIDTH);
                std::uniform_int_distribution<> y_dis(0, MAP_HEIGHT);
                std::uniform_int_distribution<> size_dis(ROOM_SIZE_MIN,
                                                         ROOM_SIZE_MAX);

                // Initialize tiles - each tile needs to know its own location
                // for pathfinding.
                for (std::size_t x = 0; x < m_map.size(); x++) {
                    for (std::size_t y = 0; y < m_map[x].size(); y++) {
                        m_map[x][y].x = x;
                        m_map[x][y].y = y;
                    }
                }

                // Create some rooms
                for (unsigned int i = 0; i < MAP_ROOMS; i++) {
                    Room room(x_dis(gen), y_dis(gen), size_dis(gen), size_dis(gen));
                    rooms.push_back(room);
                }

                // Make sure rooms aren't out-of-bounds and don't overlap.
                for (unsigned int i = 0; i < SEPARATION_ITERS; i++) {
                    bool found_intersections = false;
                    std::cout << "Room separation iteration " << i + 1
                        << std::endl;

                    for (auto&& room : rooms) {
                        unsigned int intersect_count = 0;
                        int total_adjust_x = 0;
                        int total_adjust_y = 0;
                        int adjust_x;
                        int adjust_y;

                        // Check for intersections with other rooms.
                        // TODO: efficient lookup for collisions with other
                        // rooms, rather than scanning the whole lot - possibly
                        // only worth it for large room numbers.
                        for (auto&& other : rooms) {
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
                        break;
                    }
                }

                // Discard any rooms that are out of bounds or intersecting
                // other rooms. Remove out-of-bounds rooms first, to ensure
                // that the minimal number of rooms are removed in the case
                // where there are rooms that are out-of-bounds and also
                // intersecting other rooms.
                rooms.erase(std::remove_if(
                                rooms.begin(), rooms.end(),
                                [this](Room r) { return RoomOutOfBounds(r); }),
                            rooms.end());

                auto roomIter = rooms.begin();
                while (roomIter != rooms.end()) {
                    bool erased = false;
                    auto &room = *roomIter;

                    for (auto&& other : rooms) {
                        if (&room != &other && room.Intersects(other)) {
                            roomIter = rooms.erase(roomIter);
                            erased = true;
                        }
                    }

                    if (!erased) {
                        ++roomIter;
                    }
                }

                // Fill in the map squares taken by rooms.
                for (auto &&room : rooms) {
                    for (int x = room.Left(); x < room.Right(); x++) {
                        for (int y = room.Top(); y < room.Bottom(); y++) {
                            m_map[x][y].filled = true;
                        }
                    }
                }

                // Use an Urquhart graph to determine the connections between
                // the rooms.
                using namespace graph;
                std::vector<Vec2f> centers;
                for(auto &&room : rooms) {
                    centers.push_back(Vec2f(room.CenterX(), room.CenterY()));
                }
                tris = generateDelaunay(centers);
                edges = generateUrquhart(centers);

                // Use A* to plot paths between the rooms.
                for (auto &&edge : edges) {
                    Tile *start = &m_map[static_cast<int>(edge.p1.x)]
                                            [static_cast<int>(edge.p1.y)];
                    Tile *end = &m_map[static_cast<int>(edge.p2.x)]
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

            void Draw(SDL_Renderer *renderer) const
            {
                SDL_Rect rect;

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                for (std::size_t x = 0; x < m_map.size(); x++) {
                    for (std::size_t y = 0; y < m_map[x].size(); y++) {
                        if (m_map[x][y].filled) {
                            rect.x = x * TILE_WIDTH;
                            rect.y = y * TILE_HEIGHT;
                            rect.w = TILE_WIDTH;
                            rect.h = TILE_HEIGHT;

                            SDL_RenderFillRect(renderer, &rect);
                        }
                    }
                }

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                for (auto &&tri : tris) {
                    for (auto &&edge : tri.GetEdges()) {
                        SDL_RenderDrawLine(renderer,
                                           edge.p1.x * TILE_WIDTH,
                                           edge.p1.y * TILE_HEIGHT,
                                           edge.p2.x * TILE_WIDTH,
                                           edge.p2.y * TILE_HEIGHT);
                    }
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                for (auto &&edge : edges) {
                    SDL_RenderDrawLine(renderer,
                                       edge.p1.x * TILE_WIDTH,
                                       edge.p1.y * TILE_HEIGHT,
                                       edge.p2.x * TILE_WIDTH,
                                       edge.p2.y * TILE_HEIGHT);
                }
            }

        private:
            static const unsigned int MAP_ROOMS = 20;
            static const unsigned int MAP_WIDTH = 160;
            static const unsigned int MAP_HEIGHT = 120;
            static const unsigned int TILE_WIDTH = 5;
            static const unsigned int TILE_HEIGHT = 5;
            static const unsigned int SEPARATION_ITERS = 20;
            static const unsigned int ROOM_SIZE_MIN = 8;
            static const unsigned int ROOM_SIZE_MAX = 32;

            std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> m_map;
            std::vector<graph::Edge> edges;
            std::vector<graph::Triangle> tris;

            std::vector<Tile *> GetTileNeighbours (Tile *tile)
            {
                std::vector<Tile *> neighbours;

                if (tile->x > 0) {
                    neighbours.push_back(
                        &m_map[tile->x - 1][tile->y]);
                }

                if (tile->x < MAP_WIDTH - 1) {
                    neighbours.push_back(
                        &m_map[tile->x + 1][tile->y]);
                }

                if (tile->y > 0) {
                    neighbours.push_back(
                        &m_map[tile->x][tile->y - 1]);
                }

                if (tile->y < MAP_HEIGHT - 1) {
                    neighbours.push_back(
                        &m_map[tile->x][tile->y + 1]);
                }

                return neighbours;
            }

            int
            PathHeuristic (Tile *tile, Tile *goal)
            {
                int x_diff = static_cast<int>(tile->x) -
                                                static_cast<int>(goal->x);
                int y_diff = static_cast<int>(tile->y) -
                                                static_cast<int>(goal->y);
                return x_diff * x_diff + y_diff * y_diff;
            }

            bool RoomOutOfBounds (Room &room,
                                  int  *adjust_x = NULL,
                                  int  *adjust_y = NULL)
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
    };
}


int
main (int argc, char *argv[])
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    dungeon::Map  map;
    bool          run;

    // TODO: Error handling
    (void)SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Dungeon",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              800, 600,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    run = true;
    while (run) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        map.Draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
