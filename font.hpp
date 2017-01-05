#ifndef __FONT_H__
#define __FONT_H__

#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <string>


namespace dungeon
{
    class Font
    {
        public:
            Font(const char *fontName, unsigned int height);
            Font(const char *fontName, unsigned int height, const char *text)
                : Font(fontName, height)
            {
                SetText(text);
            }

            Font(const char *fontName, unsigned int height, const char *text,
                 unsigned int x, unsigned int y)
                : Font(fontName, height, text)
            {
                m_x = x;
                m_y = y;
            }

            ~Font();

            void Print(const char *text, unsigned int x, unsigned int y)
            {
                SetText(text);
                DrawText(x, y);
            }

            void SetText(const char *text);
            void SetCoords(unsigned int x, unsigned int y)
            {
                m_x = x;
                m_y = y;
            }

            void DrawText(unsigned int x, unsigned int y) const;
            void DrawText() const
            {
                DrawText(m_x, m_y);
            }

            unsigned int GetWidth() const
            {
                return m_width;
            }

            unsigned int GetHeight() const
            {
                return m_height;
            }

        private:
            TTF_Font     *m_font;
            GLuint        m_texId;
            unsigned int  m_width;
            unsigned int  m_height;
            unsigned int  m_x;
            unsigned int  m_y;
    };
}

#endif // __FONT_H__
