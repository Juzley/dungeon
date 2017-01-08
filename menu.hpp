#ifndef __MENU_HPP__
#define __MENU_HPP__


#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "gamestate.hpp"
#include "font.hpp"


namespace dungeon
{
    class MenuItem
    {
        public:
            MenuItem(unsigned int id) : m_id(id)
            {
            }

            virtual ~MenuItem()
            {
            }

            unsigned int GetId() {
                return m_id;
            }
            
            virtual void Draw(bool selected) const = 0;

        protected:
            int m_id;
    };


    class TextMenuItem : public MenuItem
    {
        public:
            TextMenuItem(const char *text, unsigned int x, unsigned int y,
                         unsigned int height, SDL_Color color, int id)
                : MenuItem(id),
                // TODO: Bundle a font with the game.
                  m_font("/usr/share/fonts/truetype/freefont/FreeMono.ttf",
                         height, text, x, y)
            {
            }

            ~TextMenuItem()
            {
            }

            void Draw(bool selected) const override 
            {
                if (selected) {
                    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                } else {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                }
                m_font.DrawText();
            }

        private:
            Font m_font;
    };


    class Menu : public GameState
    {
        public:
            Menu() : m_selectedItem(0) {}
            virtual ~Menu() {}
            virtual void Run() override;
            virtual void Draw() const override;
            virtual void OnActivateItem(int id) = 0;

        protected:
            std::vector<std::shared_ptr<MenuItem>> m_items;
            std::vector<std::shared_ptr<MenuItem>>::size_type m_selectedItem;
    };
}


#endif // __MENU_HPP__
