#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

#include "graph.hpp"
#include "generator.hpp"
#include "gamestate.hpp"


int
main (int argc, char *argv[])
{
    SDL_Window                *window;
    SDL_Renderer              *renderer;
    dungeon::Generator         generator;
    dungeon::GameStateManager  gameState;
    bool                       run;

    // TODO: Error handling
    (void)SDL_Init(SDL_INIT_VIDEO);
    (void)TTF_Init();
    window = SDL_CreateWindow("Dungeon",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              1024, 768,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Set up the gamestate manager and push the initial gamestate.
    gameState.Push(std::make_shared<dungeon::GeneratorGameState>(
                                                        renderer, gameState));

    run = true;
    while (run) {
        // Run the current gamestate
        gameState.Run();

        // Draw the current gamestate
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        gameState.Draw(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
