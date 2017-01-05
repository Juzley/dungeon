#ifndef __CONTROLMENU_HPP__
#define __CONTROLMENU_HPP__


#include <SDL2/SDL.h>
#include "menu.hpp"
#include "font.hpp"


namespace dungeon
{

    class ControlMenuItem : public MenuItem
    {
        public:
            ControlMenuItem(const char *text, unsigned int x, unsigned int y,
                            unsigned int id)
                : MenuItem(id),
                  m_font("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 10, text, x, y),
                  m_x(x), m_y(y)
            {
            }

            void Draw(bool selected) const override
            {
                m_font.DrawText();
            }

        private:
            Font         m_font;
            unsigned int m_x;
            unsigned int m_y;
    };


    class ControlMenu : public Menu
    {
        public:
            ControlMenu(GameStateManager &manager);
            void OnActivateItem(int id) override;

        private:
            enum {
                CONTROL_UP,
                CONTROL_LEFT,
                CONTROL_DOWN,
                CONTROL_RIGHT
            };

            GameStateManager &m_manager;
    };
}


#endif // __CONTROLMENU_HPP__
