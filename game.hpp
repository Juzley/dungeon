#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <SDL2/SDL.h>

#include "gamestate.hpp"
#include "map.hpp"
#include "player.hpp"


namespace dungeon
{
    class Game : public GameState
    {
        public:
            Game(SDL_Renderer *renderer, GameStateManager &manager,
                 std::shared_ptr<Map> map)
                : m_manager(manager), m_renderer(renderer), m_map(map)
            {
            }

            void Run() override;
            void Draw(SDL_Renderer *renderer) const override;
            void Start() override;

        private:
            void UpdateVisibility();
            void DrawMiniMap(SDL_Renderer *renderer) const;

            GameStateManager     &m_manager;
            SDL_Renderer         *m_renderer;
            std::shared_ptr<Map>  m_map;
            Player                m_player;
    };
}


#endif
