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
        : m_manager(manager),
          m_listening(false),
          m_upText("/usr/share/fonts/truetype/freefont/FreeMono.ttf",
                   32, "Move Up", 0, 0),
          m_downText("/usr/share/fonts/truetype/freefont/FreeMono.ttf",
                     32, "Move Down", 0, 40),
          m_leftText("/usr/share/fonts/truetype/freefont/FreeMono.ttf",
                     32, "Move Left", 0, 80),
          m_rightText("/usr/share/fonts/truetype/freefont/FreeMono.ttf",
                      32, "Move Right", 0, 120)
    {
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(
                SDL_GetKeyName(g_settings.GetControl(Settings::CONTROL_UP)),
                32, 400, 0,
                ItemForControl(Settings::CONTROL_UP))));
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(
                SDL_GetKeyName(g_settings.GetControl(Settings::CONTROL_DOWN)),
                32, 400, 40,
                ItemForControl(Settings::CONTROL_DOWN))));
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(
                SDL_GetKeyName(g_settings.GetControl(Settings::CONTROL_LEFT)),
                32, 400, 80,
                ItemForControl(Settings::CONTROL_LEFT))));
        m_items.push_back(std::shared_ptr<ControlMenuItem>(
            new ControlMenuItem(
                SDL_GetKeyName(g_settings.GetControl(Settings::CONTROL_RIGHT)),
                32, 400, 120,
                ItemForControl(Settings::CONTROL_RIGHT))));
        m_items.push_back(std::shared_ptr<TextMenuItem>(
            new TextMenuItem("Back", 400, 400, 32, { 255, 255, 255, 255 },
                             ITEM_BACK)));
    }


    void ControlMenu::Draw() const
    {
        std::pair<const Font *, Settings::Control> labels[] = {
            std::make_pair(&m_upText, Settings::CONTROL_UP),
            std::make_pair(&m_downText, Settings::CONTROL_DOWN),
            std::make_pair(&m_leftText, Settings::CONTROL_LEFT),
            std::make_pair(&m_rightText, Settings::CONTROL_RIGHT)
        };

        for (size_t i = 0; i < (sizeof(labels) / sizeof(*labels)); i++) {
            if (m_items[m_selectedItem]->GetId() ==
                static_cast<unsigned int>(ItemForControl(labels[i].second))) {
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            } else {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            }
            labels[i].first->DrawText();
        }

        Menu::Draw();
    }


    void ControlMenu::OnActivateItem(int id)
    {
        if (id == ITEM_BACK) {
            m_manager.Pop();
        } else {
            m_listening = true;
            m_listeningItem = std::static_pointer_cast<ControlMenuItem>(
                                                    m_items[m_selectedItem]);
            m_listeningItem->Activate();
        }
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
                        g_settings.SetControl(
                            ControlForItem(m_listeningItem->GetId()),
                            e.key.keysym.sym);
                        g_settings.Save();
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
