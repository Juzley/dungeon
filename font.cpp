#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include "font.hpp"

namespace dungeon
{
    Font::Font(const char *fontName, unsigned int height)
    {
        // TODO: Media manager
        // TODO: Bundle a font with the game - this only works on
        // Ubuntu currently.
        TTF_Font *m_font = TTF_OpenFont(fontName, height);
        if (m_font == NULL) {
            // TODO: Exception
        }

        glGenTextures(1, &m_texId);
    }


    Font::~Font()
    {
        glDeleteTextures(1, &m_texId);
        TTF_CloseFont(m_font);
    }


    static unsigned int next_power_two(unsigned int val) {
        unsigned int power = 2;
        unsigned int nextVal = power * 2;

        while ((nextVal *= 2) <= val) {
            power*=2;
        }

        return power*2;
    }


    void Font::SetText(const char *text)
    {
        SDL_Color color = { 255, 255, 255, 255};
        SDL_Surface *renderedText;

        // Render font to a SDL_Surface
        renderedText = TTF_RenderText_Blended(m_font, text, color);
        if (renderedText == NULL) {
            // TODO: Exception
        }

        // Save the width and height of the actual text for drawing later.
        m_width = renderedText->w;
        m_height = renderedText->h;

        // Create a surface large enough with dimensions which are powers of
        // two, and copy the text into it.
        unsigned int w = next_power_two(renderedText->w) * 2;
        unsigned int h = next_power_two(renderedText->h) * 2;
        SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32,
            0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
        if (surface == NULL) {
            // TODO: Exception
        }
        SDL_BlitSurface(renderedText, NULL, surface, NULL);

        // Upload the texture.
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, surface->pixels);
        glDisable(GL_TEXTURE_2D);

        SDL_FreeSurface(surface);
        SDL_FreeSurface(renderedText);
    }


    void Font::DrawText(unsigned int x, unsigned int y) const
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 1); glVertex2f(x,           y);
            glTexCoord2i(1, 1); glVertex2f(x + m_width, y);
            glTexCoord2i(1, 0); glVertex2f(x + m_width, y + m_height);
            glTexCoord2i(0, 0); glVertex2f(x,           y + m_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);

    }
}
