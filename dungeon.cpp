#include <SDL2/SDL.h>
#include <array>
#include <random>
#include <cmath>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <utility>


namespace dungeon
{
    struct Room
    {
        public:
            Room(unsigned int x,
                 unsigned int y,
                 unsigned int width,
                 unsigned int height)
                : x(x), y(y), width(width), height(height)
            {
            }

            // @@@
            bool Intersects (Room &other)
            {
                if (x + width < other.x || x > other.x + other.width ||
                    y < other.y + other.height || y + height > other.y) {
                    // Intersects

                    if (x + width > other.x + other.width) {
                        // Move (other.right - x)
                    } else {
                        // Move (other.x - right)
                    }
                    
                    if (y + height > other.y + other.height) {
                        // Move (other.bottom - y)
                    } else {
                        // Move (other.y - bottom)
                    }

                    return (true);
                }

                return (false);
            }

            unsigned int x;
            unsigned int y;
            unsigned int width;
            unsigned int height;
    };


    struct Tile
    {
        public:
            Tile() : nonempty(false)
            {
            }

            unsigned int x;
            unsigned int y;
            bool nonempty;
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
                std::uniform_int_distribution<> x_dis(0, MAP_WIDTH - 6);
                std::uniform_int_distribution<> y_dis(0, MAP_HEIGHT - 6);

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
                    Room room(x_dis(gen), y_dis(gen), 6, 6);
                    rooms.push_back(room);
                }

                // Fill in the squares on the map.
                for (auto&& room : rooms) {
                    for (unsigned int x = room.x;
                         x < room.x + room.width;
                         x++) {
                        for (unsigned int y = room.y;
                             y < room.y + room.height;
                             y++) {
                            m_map[x][y].nonempty = true;
                        }
                    }
                }

                // Find a path between the first two rooms
                Room &start_room = rooms[0];
                Room &end_room = rooms[1];
                Tile *start = &m_map[start_room.x + start_room.width / 2]
                                        [start_room.y + start_room.height / 2];
                Tile *end = &m_map[end_room.x + end_room.width / 2]
                                            [end_room.y + end_room.height / 2];

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
                    current->nonempty = true;
                    current = came_from[current];
                }
            }

            void Draw(SDL_Renderer *renderer) const
            {
                SDL_Rect rect;

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                for (std::size_t x = 0; x < m_map.size(); x++) {
                    for (std::size_t y = 0; y < m_map[x].size(); y++) {
                        if (m_map[x][y].nonempty) {
                            rect.x = x * TILE_WIDTH;
                            rect.y = y * TILE_HEIGHT;
                            rect.w = TILE_WIDTH;
                            rect.h = TILE_HEIGHT;

                            SDL_RenderFillRect(renderer, &rect);
                        }
                    }
                }
            }

        private:
            static const unsigned int MAP_ROOMS = 20;
            static const unsigned int MAP_WIDTH = 160;
            static const unsigned int MAP_HEIGHT = 120;
            static const unsigned int TILE_WIDTH = 5;
            static const unsigned int TILE_HEIGHT = 5;

            std::vector<Tile *>
            GetTileNeighbours(Tile *tile)
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
                        &m_map[tile->x][tile->y - 1]);
                }

                return (neighbours);
            }

            int
            PathHeuristic(Tile *tile, Tile *goal)
            {
                int h = std::abs(static_cast<int>(tile->x) -
                                 static_cast<int>(goal->x)) +
                        std::abs(static_cast<int>(tile->y) -
                                 static_cast<int>(goal->y));

                // Prefer to go through empty space.
                if (tile->nonempty) {
                    h *= 2;
                }
                
                return h;
            }

            std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> m_map;
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

    return (0);
}
