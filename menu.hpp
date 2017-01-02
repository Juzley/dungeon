#ifndef __MENU_HPP__
#define __MENU_HPP__


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>
#include "gamestate.hpp"


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
            
            virtual void Draw(SDL_Renderer *renderer, bool selected) const = 0;

        protected:
            int m_id;
    };


    class TextMenuItem : public MenuItem
    {
        public:
            TextMenuItem(SDL_Renderer *renderer, const char *text,
                         unsigned int x, unsigned int y, unsigned int height,
                         SDL_Color color, int id)
                : MenuItem(id)
            {
                // TODO: Media manager for fonts.
                // TODO: Bundle a font with the game - this only works on
                // Ubuntu currently.
                m_font = TTF_OpenFont(
                    "/usr/share/fonts/truetype/freefont/FreeMono.ttf", height);
                if (m_font == NULL) {
                    // TODO: Exception
                }

                SDL_Surface *surf = TTF_RenderText_Solid(m_font, text, color);
                if (surf == NULL) {
                    // TODO: Exception
                }

                m_rect.x = x;
                m_rect.y = y;
                m_rect.w = surf->w;
                m_rect.h = surf->h;

                m_texture = SDL_CreateTextureFromSurface(renderer, surf);
                if (m_texture == NULL) {
                    // TODO: Exception
                }

                SDL_FreeSurface(surf);
                TTF_CloseFont(m_font);
            }

            ~TextMenuItem()
            {
                SDL_DestroyTexture(m_texture);
            }

            void Draw(SDL_Renderer *renderer, bool selected) const override 
            {
                SDL_RenderCopy(renderer, m_texture, NULL, &m_rect);
            }

        private:
            SDL_Rect      m_rect;
            TTF_Font     *m_font;
            SDL_Texture  *m_texture;
    };


    class Menu : public GameState
    {
        public:
            Menu() : m_selectedItem(0) {}
            virtual ~Menu() {}
            virtual void Run() override;
            virtual void Draw(SDL_Renderer *renderer) const override;
            virtual void OnActivateItem(int id) = 0;

        protected:
            std::vector<std::shared_ptr<MenuItem>> m_items;
            std::vector<std::shared_ptr<MenuItem>>::size_type m_selectedItem;
    };
}


#endif // __MENU_HPP__
