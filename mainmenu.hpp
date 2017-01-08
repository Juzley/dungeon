#ifndef __MAINMENU_H__
#define __MAINMENU_H__


#include "generator.hpp"
#include "menu.hpp"


namespace dungeon {
    class MainMenu : public Menu
    {
        public:
            enum {
                ITEM_START,
                ITEM_QUIT
            };

            MainMenu(GameStateManager &manager)
                : m_manager(manager)
            {
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem("Start",
                                     400, 350, 32,
                                     { 255, 255, 255, 255 },
                                     ITEM_START)));
                m_items.push_back(std::shared_ptr<TextMenuItem>(
                    new TextMenuItem("Quit",
                                     400, 400, 32,
                                     { 255, 255, 255, 255 },
                                     ITEM_QUIT)));
            }

            void OnActivateItem(int id) override
            {
                switch (id) {
                case ITEM_START:
                    m_manager.Push(std::make_shared<GeneratorGameState>(m_manager));
                    break;

                case ITEM_QUIT:
                    m_manager.Pop();
                    break;
                }
            }

        private:
            GameStateManager &m_manager;
    };
}


#endif // __MAINMENU_H__
