#ifndef __PAUSEMENU_H__
#define __PAUSEMENU_H__

#include <SDL2/SDL.h>
#include <memory>
#include "menu.hpp"


namespace dungeon {
    class PauseMenu : public Menu
    {
        public:
            enum {
                ITEM_RESUME
            };

            PauseMenu(SDL_Renderer *renderer, GameStateManager &manager)
                : m_manager(manager)
            {
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem(renderer,
                                     "Resume",
                                     400, 400, 32,
                                     { 255, 255, 255, 255},
                                     PauseMenu::ITEM_RESUME)));
            }

            void OnActivateItem(int id) override
            {
                switch (id) {
                case ITEM_RESUME:
                    m_manager.Pop();
                    break;
                }
            }

        private:

            GameStateManager &m_manager;
    };
}


#endif // __PAUSEMENU_H__
