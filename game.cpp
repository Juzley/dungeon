#include <SDL2/SDL.h>
#include "game.hpp"
#include "map.hpp"

namespace dungeon
{
    void Game::Run()
    {
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (m_player_y > 0) {
                        m_player_y--;
                    }
                    break;

                case SDLK_DOWN:
                    if (m_player_y < Map::MAP_HEIGHT - 1) {
                        m_player_y++;
                    }
                    break;

                case SDLK_LEFT:
                    if (m_player_x > 0) {
                        m_player_x--;
                    }
                    break;

                case SDLK_RIGHT:
                    if (m_player_x < Map::MAP_WIDTH - 1) {
                        m_player_x++;
                    }
                    break;
                }
            }
        }
    }


    void Game::Draw(SDL_Renderer *renderer) const
    {
        const unsigned int TILE_SIZE = 20;
        const unsigned int TILE_COUNT_X = 39;
        const unsigned int TILE_COUNT_Y = 29;

        // Work out the start of the tiles we're going to draw.
        unsigned int start_x = (TILE_COUNT_X - 1) / 2 > m_player_x ?
            0 : m_player_x - (TILE_COUNT_X - 1) / 2;
        unsigned int start_y = (TILE_COUNT_Y - 1) / 2 > m_player_y ?
            0 : m_player_y - (TILE_COUNT_Y - 1) / 2;

        if (start_x + TILE_COUNT_X >= Map::MAP_WIDTH) {
            start_x = Map::MAP_WIDTH - TILE_COUNT_X - 1;
        }

        if (start_y + TILE_COUNT_Y >= Map::MAP_HEIGHT) {
            start_y = Map::MAP_HEIGHT - TILE_COUNT_Y - 1;
        }

        for (Map::TileIter tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            if (tileIter->x >= start_x &&
                tileIter->x < start_x + TILE_COUNT_X &&
                tileIter->y >= start_y &&
                tileIter->y < start_y + TILE_COUNT_Y) {

                if (!tileIter->IsEmpty()) {
                    if (tileIter->type == Tile::FLOOR) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    } else if (tileIter->type == Tile::WALL) {
                        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
                    }

                    SDL_Rect rect = { .x = static_cast<int>((tileIter->x - start_x) * TILE_SIZE),
                                      .y = static_cast<int>((tileIter->y - start_y) * TILE_SIZE),
                                      .w = TILE_SIZE,
                                      .h = TILE_SIZE };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }
}
