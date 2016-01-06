#include <SDL2/SDL.h>
#include <array>
#include <cmath>

namespace dungeon
{
    class Map
    {
        public:
            Map ()
            {
                // TODO: Initialize map
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
