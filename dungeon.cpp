#include <SDL2/SDL.h>
#include <array>
#include <random>
#include <cmath>

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

            unsigned int x;
            unsigned int y;
            unsigned int width;
            unsigned int height;
    };


    class Map
    {
        public:
            Map ()
            {
                std::vector<Room> rooms;
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> x_dis(0, MAP_WIDTH);
                std::uniform_int_distribution<> y_dis(0, MAP_HEIGHT);

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
                            m_map[y * MAP_WIDTH + x] = true;
                        }
                    }
                }
            }

            void Draw(SDL_Renderer *renderer) const
            {
                SDL_Rect rect;

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                for (std::size_t i = 0; i < m_map.size(); i++) {
                    if (m_map[i]) {
                        unsigned int x = i % MAP_WIDTH;
                        unsigned int y = static_cast<unsigned int>(
                                    std::floor(static_cast<float>(i) /
                                               static_cast<float>(MAP_WIDTH)));

                        rect.x = x * TILE_WIDTH;
                        rect.y = y * TILE_HEIGHT;
                        rect.w = TILE_WIDTH;
                        rect.h = TILE_HEIGHT;

                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }

        private:



            static const unsigned int MAP_ROOMS = 20;
            static const unsigned int MAP_WIDTH = 160;
            static const unsigned int MAP_HEIGHT = 120;
            static const unsigned int TILE_WIDTH = 5;
            static const unsigned int TILE_HEIGHT = 5;

            std::array<bool, MAP_WIDTH * MAP_HEIGHT> m_map;
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
