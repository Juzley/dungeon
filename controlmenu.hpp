#ifndef __CONTROLMENU_HPP__
#define __CONTROLMENU_HPP__


#include <SDL2/SDL.h>
#include "menu.hpp"
#include "font.hpp"
#include "settings.hpp"


namespace dungeon
{

    class ControlMenuItem : public MenuItem
    {
        public:
            ControlMenuItem(const char *text, unsigned int height, unsigned int x, unsigned int y,
                            unsigned int id)
                : MenuItem(id),
                  m_font("/usr/share/fonts/truetype/freefont/FreeMono.ttf", height, text, x, y),
                  m_x(x), m_y(y)
            {
            }

            void Draw(bool selected) const override;

            void Activate(bool active=true)
            {
                m_active = active;
            }

            void SetText(const char *text)
            {
                m_font.SetText(text);
            }

        private:
            Font         m_font;
            unsigned int m_x;
            unsigned int m_y;
            bool         m_active;
    };


    class ControlMenu : public Menu
    {
        public:
            ControlMenu(GameStateManager &manager);
            void OnActivateItem(int id) override;
            void Run() override;

        private:
            enum {
                ITEM_BACK,
                ITEM_CONTROLS
            };

            int ItemForControl(Settings::Control c) {
                return ITEM_CONTROLS + c;
            }

            Settings::Control ControlForItem(int item)
            {
                return static_cast<Settings::Control>(
                                    item - static_cast<int>(ITEM_CONTROLS));
            }

            GameStateManager                 &m_manager;
            bool                              m_listening;
            std::shared_ptr<ControlMenuItem>  m_listeningItem;
    };
}


#endif // __CONTROLMENU_HPP__
