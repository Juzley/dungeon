#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <memory>

#include "graph.hpp"
#include "gamestate.hpp"
#include "mainmenu.hpp"
#include "settings.hpp"


int
main (int argc, char *argv[])
{
    SDL_Event                  e;
    SDL_Window                *window;
    SDL_GLContext              glContext;
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
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    glViewport(0, 0, 1024, 768);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, 1024, 768, 0, -1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

    // Load settings
    dungeon::g_settings.Load();

    // Set up the gamestate manager and push the initial gamestate.
    gameState.Push(std::make_shared<dungeon::MainMenu>(gameState));

    run = true;
    while (run) {
        // Check for exit event - just peek the event queue as gamestates will
        // generally use SDL_PollEvent to get events.
        if (SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0) {
            run = false;
        }

        // Run the current gamestate
        gameState.Run();

        // Draw the current gamestate
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gameState.Draw();
        SDL_GL_SwapWindow(window);

        // Removing the final gamestate indicates that we are exiting.
        if (gameState.IsEmpty()) {
            run = false;
        }
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
