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
#include "generator.hpp"


int
main (int argc, char *argv[])
{
    SDL_Window         *window;
    SDL_Renderer       *renderer;
    dungeon::Generator  generator;
    bool                run;

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

        // TODO: Delay iteration with timer/keypresses
        generator.Iterate();
        generator.Draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
