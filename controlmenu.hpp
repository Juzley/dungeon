#ifndef __CONTROLMENU_HPP__
#define __CONTROLMENU_HPP__


#include <SDL2/SDL.h>
#include "menu.hpp"


namespace dungeon
{

    class ControlMenuItem : public MenuItem
    {
        public:
            ControlMenuItem(unsigned int id) : MenuItem(id) {}

            void Draw(SDL_Renderer *renderer, bool selected) const override;
    };


    class ControlMenu : public Menu
    {
        public:
            void OnActivateItem(int id) override;

        private:
            enum {
                CONTROL_UP,
                CONTROL_LEFT,
                CONTROL_DOWN,
                CONTROL_RIGHT
            };
    };
}


#endif // __CONTROLMENU_HPP__
