#include <SDL2/SDL.h>
#include "gamestate.hpp"
#include "controlmenu.hpp"


namespace dungeon
{
    ControlMenu::ControlMenu(GameStateManager &manager)
        : m_manager(manager)
    {
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(SDL_GetKeyName(SDLK_UP), 0, 0, CONTROL_UP)));
    }


    void ControlMenu::OnActivateItem(int id)
    {
    }
}
