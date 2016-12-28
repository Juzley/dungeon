#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <SDL2/SDL.h>

#include "gamestate.hpp"
#include "map.hpp"


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

        private:
            GameStateManager     &m_manager;
            std::shared_ptr<Map>  m_map;
            unsigned int          m_player_x;
            unsigned int          m_player_y;
    };
}


#endif
