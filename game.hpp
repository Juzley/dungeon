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
            Game(GameStateManager manager, std::shared_ptr<Map> map)
                : m_manager(manager), m_map(map)
            {
            }

            void Run() override;
            void Draw(SDL_Renderer *renderer) const override;
            void Start() override;

        private:
            void UpdateVisibility();

            GameStateManager     &m_manager;
            std::shared_ptr<Map>  m_map;
            Player                m_player;
    };
}


#endif
