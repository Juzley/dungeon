#ifndef __PAUSEMENU_H__
#define __PAUSEMENU_H__

#include <SDL2/SDL.h>
#include <memory>
#include "menu.hpp"
#include "controlmenu.hpp"


namespace dungeon {
    class PauseMenu : public Menu
    {
        public:
            enum {
                ITEM_CONTROLS,
                ITEM_RESUME,
                ITEM_QUIT
            };

            PauseMenu(GameStateManager &manager)
                : m_manager(manager)
            {
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem("Controls",
                                     400, 350, 32,
                                     { 255, 255, 255, 255 },
                                     ITEM_CONTROLS)));
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem("Resume",
                                     400, 400, 32,
                                     { 255, 255, 255, 255 },
                                     ITEM_RESUME)));
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem("Quit",
                                     400, 450, 32,
                                     { 255, 255, 255, 255 },
                                     ITEM_QUIT)));
            }

            void OnActivateItem(int id) override
            {
                switch (id) {
                case ITEM_RESUME:
                    m_manager.Pop();
                    break;

                case ITEM_CONTROLS:
                    m_manager.Push(std::make_shared<ControlMenu>(m_manager));
                    break;

                case ITEM_QUIT:
                    // Go back to the main menu - pop both this and the game
                    // gamestate.
                    m_manager.Pop(2);
                    break;
                }
            }

        private:
            GameStateManager &m_manager;
    };
}


#endif // __PAUSEMENU_H__
