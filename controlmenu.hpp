#ifndef __CONTROLMENU_HPP__
#define __CONTROLMENU_HPP__


#include <SDL2/SDL.h>
#include "menu.hpp"


namespace dungeon
{

    class ControlMenuItem : public MenuItem
    {
        public:
            void Draw(SDL_Renderer *renderer, bool selected) const override;
    };


    class ControlMenu : public Menu
    {
        public:
            void OnActivateItem(int id) override;

        private:
    };
}


#endif // __CONTROLMENU_HPP__
