#include <SDL2/SDL.h>
#include "menu.hpp"


namespace dungeon
{
    void Menu::Run() {
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (m_selectedItem == 0) {
                            m_selectedItem = m_items.size() - 1;
                        } else {
                            m_selectedItem--;
                        }
                        break;

                    case SDLK_DOWN:
                        if (m_selectedItem == m_items.size() - 1) {
                            m_selectedItem = 0;
                        } else {
                            m_selectedItem++;
                        }
                        break;

                    case SDLK_RETURN:
                        OnActivateItem(m_items[m_selectedItem]->GetId());
                        break;
                }
            }
        }
    }


    void Menu::Draw() const
    {
        for (auto &&item : m_items) {
            // Work out if this is the selected item.
            bool selected = (item == m_items[m_selectedItem]);
            item->Draw(selected);
        }
    }
}
