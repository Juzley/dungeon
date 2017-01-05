#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <string>


namespace dungeon
{
    class Font
    {
        public:
            Font(const char *fontName, unsigned int height);
            ~Font();

            void Print(const char *text, unsigned int x, unsigned int y)
            {
                SetText(text);
                DrawText(x, y);
            }

            void SetText(const char *text);
            void DrawText(unsigned int x, unsigned int y);

        private:
            TTF_Font     *m_font;
            GLuint        m_texId;
            unsigned int  m_width;
            unsigned int  m_height;
    };
}
