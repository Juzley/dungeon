#include <SDL2/SDL.h>
#include "game.hpp"
#include "map.hpp"

namespace dungeon
{
    void Game::Start()
    {
        // Find a spawn point.
        for (auto tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            if (tileIter->spawn) {
                m_player.x = tileIter->x;
                m_player.y = tileIter->y;
            }
        }
    }

    void Game::Run()
    {
        SDL_Event e;

        int newX = static_cast<int>(m_player.x);
        int newY = static_cast<int>(m_player.y);

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    newY--;
                    break;
                case SDLK_DOWN:
                    newY++;
                    break;

                case SDLK_LEFT:
                    newX--;
                    break;

                case SDLK_RIGHT:
                    newX++;
                    break;
                }
            }
        }

        if (newX > 0 && newX < static_cast<int>(Map::MAP_WIDTH - 1) &&
            newY > 0 && newY < static_cast<int>(Map::MAP_HEIGHT - 1)) {
            if (m_map->GetTile(newX, newY).type == Tile::FLOOR) {
                m_player.x = newX;
                m_player.y = newY;
            }
        }
    }


    void Game::Draw(SDL_Renderer *renderer) const
    {
        const unsigned int TILE_SIZE = 20;
        const unsigned int TILE_COUNT_X = 39;
        const unsigned int TILE_COUNT_Y = 29;

        // Work out the start of the tiles we're going to draw.
        unsigned int start_x = (TILE_COUNT_X - 1) / 2 > m_player.x ?
            0 : m_player.x - (TILE_COUNT_X - 1) / 2;
        unsigned int start_y = (TILE_COUNT_Y - 1) / 2 > m_player.y ?
            0 : m_player.y - (TILE_COUNT_Y - 1) / 2;

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
                    if (tileIter->x == m_player.x && tileIter->y == m_player.y) {
                        SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);
                    } else if (tileIter->type == Tile::FLOOR) {
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
