#include <SDL2/SDL.h>
#include <GL/gl.h>
#include "gamestate.hpp"
#include "controlmenu.hpp"
#include "settings.hpp"


namespace dungeon
{
    void ControlMenuItem::Draw(bool selected) const
    {
        if (m_active) {
            glColor4f(0.7f, 0.7f, 1.0f, 1.0f);
        } else {
            glColor4f(0.1f, 0.1f, 0.3f, 1.0f);
        }
        glRecti(m_x, m_y, m_x + m_font.GetWidth(), m_y + m_font.GetHeight());

        if (!m_active) {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            m_font.DrawText();
        }
    }


    ControlMenu::ControlMenu(GameStateManager &manager)
        : m_manager(manager)
    {
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(SDL_GetKeyName(SDLK_UP), 20, 0, 0,
                                Settings::CONTROL_UP)));
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(SDL_GetKeyName(SDLK_DOWN), 20, 0, 50,
                                Settings::CONTROL_DOWN)));
    }


    void ControlMenu::OnActivateItem(int id)
    {
        m_listening = true;
        m_listeningItem = std::static_pointer_cast<ControlMenuItem>(
                                                    m_items[m_selectedItem]);
        m_listeningItem->Activate();
    }


    void ControlMenu::Run()
    {
        if (m_listening) {
            SDL_Event e;

            if (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        m_listening = false;
                    } else {
                        m_listeningItem->SetText(SDL_GetKeyName(e.key.keysym.sym));
                        m_listening = false;
                        m_listeningItem->Activate(false);
                        m_listeningItem.reset();
                    }
                }
            }
        } else {
            Menu::Run();
        }
    }
}
