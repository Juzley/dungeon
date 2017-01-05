#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__


#include <SDL2/SDL.h>


namespace dungeon
{
    class Settings
    {
        public:
            enum Control {
                CONTROL_UP,
                CONTROL_LEFT,
                CONTROL_DOWN,
                CONTROL_RIGHT
            };

            void SetControl(Control c, SDL_Keycode k)
            {
            }


            SDL_Keycode GetControl(Control c)
            {
                return SDLK_UP;
            }
    };
}


#endif // __SETTINGS_HPP__

