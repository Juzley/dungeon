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
                : m_manager(manager), m_map(map)
            {
            }

            void Run() override;
            void Draw() const override;
            void Start() override;

        private:
            void UpdateVisibility();
            void PickupItems();
            void DrawMiniMap() const;

            GameStateManager     &m_manager;
            std::shared_ptr<Map>  m_map;
            Player                m_player;
    };
}


#endif
