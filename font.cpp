#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <stdexcept>
#include "font.hpp"

namespace dungeon
{
    Font::Font(const char *fontName, unsigned int height)
    {
        // TODO: Media manager
        m_font = TTF_OpenFont(fontName, height);
        if (m_font == NULL) {
            throw std::runtime_error("Failed to load font");
        }

        glGenTextures(1, &m_texId);
    }


    Font::~Font()
    {
        glDeleteTextures(1, &m_texId);
        TTF_CloseFont(m_font);
    }


    void Font::SetText(const char *text)
    {
        SDL_Color color = { 255, 255, 255, 255 };

        // Render font to a SDL_Surface
        SDL_Surface *surface = TTF_RenderText_Blended(m_font, text, color);
        if (surface == NULL) {
            throw std::runtime_error("Failed to render text");
        }

        // Save the width and height of the actual text for drawing later.
        m_width = surface->w;
        m_height = surface->h;

        // Upload the texture.
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE, surface->pixels);
        glDisable(GL_TEXTURE_2D);

        SDL_FreeSurface(surface);
    }


    void Font::DrawText(unsigned int x, unsigned int y) const
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2f(x,           y);
            glTexCoord2i(1, 0); glVertex2f(x + m_width, y);
            glTexCoord2i(1, 1); glVertex2f(x + m_width, y + m_height);
            glTexCoord2i(0, 1); glVertex2f(x,           y + m_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);

    }
}
