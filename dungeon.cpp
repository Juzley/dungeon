#include <SDL2/SDL.h>

namespace dungeon
{
}


int
main (int argc, char *argv[])
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Rect      rect;
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

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        rect.x = 400;
        rect.y = 300;
        rect.w = 32;
        rect.h = 32;
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return (0);
}
