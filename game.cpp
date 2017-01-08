#include <SDL2/SDL.h>
#include <GL/gl.h>
#include "game.hpp"
#include "map.hpp"
#include "pausemenu.hpp"
#include "settings.hpp"

namespace dungeon
{
    void Game::UpdateVisibility()
    {
        const int SIGHT_DISTANCE = 256;

        // TODO: Make this more efficient - probably move into map and just
        // look at the nearby tiles.
        m_map->ResetVisibility();
        for (auto tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            int x_dist = static_cast<int>(m_player.x) - static_cast<int>(tileIter->x);
            int y_dist = static_cast<int>(m_player.y) - static_cast<int>(tileIter->y);
            if (x_dist * x_dist + y_dist * y_dist <= SIGHT_DISTANCE) {
                if (m_map->IsVisible(m_player.x, m_player.y,
                                     tileIter->x, tileIter->y)) {
                    tileIter->visible = true;
                    tileIter->seen = true;
                }
            }
        }
    }


    void Game::Start()
    {
        // Find a spawn point.
        for (auto tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            if (tileIter->spawn) {
                m_player.x = tileIter->x;
                m_player.y = tileIter->y;
                break;
            }
        }

        UpdateVisibility();
    }


    void Game::Run()
    {
        SDL_Event e;

        int newX = static_cast<int>(m_player.x);
        int newY = static_cast<int>(m_player.y);

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym ==
                    g_settings.GetControl(Settings::CONTROL_UP)) {
                    newY--;
                } else if (e.key.keysym.sym ==
                           g_settings.GetControl(Settings::CONTROL_DOWN)) {
                    newY++;
                } else if (e.key.keysym.sym ==
                           g_settings.GetControl(Settings::CONTROL_LEFT)) {
                    newX--;
                } else if (e.key.keysym.sym ==
                           g_settings.GetControl(Settings::CONTROL_RIGHT)) {
                    newX++;
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    m_manager.Push(std::make_shared<PauseMenu>(m_manager));
                }
            }
        }

        if ((newX != static_cast<int>(m_player.x) || newY != static_cast<int>(m_player.y)) &&
            newX > 0 && newX < static_cast<int>(Map::MAP_WIDTH - 1) &&
            newY > 0 && newY < static_cast<int>(Map::MAP_HEIGHT - 1)) {
            Tile &tile = m_map->GetTile(newX, newY);

            if (tile.type == Tile::FLOOR ||
                tile.type == Tile::DOOR_OPEN) {
                m_player.x = newX;
                m_player.y = newY;
                UpdateVisibility();
            } else if (tile.type == Tile::DOOR_CLOSED) {
                tile.type = Tile::DOOR_OPEN;
                UpdateVisibility();
            }
        }
    }


    void Game::DrawMiniMap() const
    {
        const unsigned int MINIMAP_START_X = 800;
        const unsigned int MINIMAP_START_Y = 400;
        const unsigned int MINIMAP_WIDTH = 200;
        const unsigned int MINIMAP_HEIGHT = 200;

        // Work out the size of a tile.
        const unsigned int tile_width = MINIMAP_WIDTH / m_map->Width();
        const unsigned int tile_height = MINIMAP_HEIGHT / m_map->Width();

        for (auto tileIter = m_map->cbeginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {

            if (!tileIter->IsEmpty() && tileIter->seen) {
                unsigned int x = tileIter->x * tile_width + MINIMAP_START_X;
                unsigned int y = tileIter->y * tile_height + MINIMAP_START_Y;
                if (tileIter->x == m_player.x && tileIter->y == m_player.y) {
                    glColor4f(1, 0, 0, 1);
                } else {
                    glColor4f(1, 1, 1, 1);
                }
                glRecti(x, y, x + tile_width, y + tile_height);

                if (!tileIter->visible) {
                    glColor4f(0, 0, 0, 0.3f);
                    glRecti(x, y, x + tile_width, y + tile_height);
                }
            }
        }
    }


    void Game::Draw() const
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

        for (auto tileIter = m_map->beginTiles();
             tileIter != m_map->endTiles();
             ++tileIter) {
            if (tileIter->x >= start_x &&
                tileIter->x < start_x + TILE_COUNT_X &&
                tileIter->y >= start_y &&
                tileIter->y < start_y + TILE_COUNT_Y) {

                if (!tileIter->IsEmpty() && tileIter->seen) {
                    if (tileIter->x == m_player.x && tileIter->y == m_player.y) {
                        glColor4f(0.8f, 0.3f, 0.3f, 1.0f);
                    } else if (tileIter->type == Tile::FLOOR ||
                               tileIter->type == Tile::DOOR_OPEN) {
                        glColor4f(1, 1, 1, 1);
                    } else if (tileIter->type == Tile::WALL) {
                        glColor4f(0.3f, 0.3f, 0.8f, 1.0f);
                    } else if (tileIter->type == Tile::DOOR_CLOSED) {
                        glColor4f(0, 0.3f, 0, 1);
                    }

                    unsigned int x = (tileIter->x - start_x) * TILE_SIZE;
                    unsigned int y = (tileIter->y - start_y) * TILE_SIZE;
                    glRecti(x, y, x + TILE_SIZE, y + TILE_SIZE);
                    
                    if (!tileIter->visible) {
                        glColor4f(0, 0, 0, 0.3f);
                        glRecti(x, y, x + TILE_SIZE, y + TILE_SIZE);
                    }
                }
            }
        }

        DrawMiniMap();
    }
}
