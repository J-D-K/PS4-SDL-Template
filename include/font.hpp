#pragma once
#include "texture.hpp"
#include <SDL2/SDL.h>
#include <map>
#include <proto-include.h> // I guess this is the header for using sceFreetype?
#include <string>
#include <vector>

// Normally I like this in all separate headers/files but in this case I don't see the point for only two functions.
namespace font
{
    // Loads module, initializes freetype.
    bool initialize(void);
    // Exits freetype
    void exit(void);

    // Struct that holds data needed to render glyphs correctly.
    typedef struct
    {
            int left, top;
            int advanceX;
            SDL::sdlTexture glyphTexture;
    } glyphData;

    // Loads font to memory, caches glyphs, and
    class ttf
    {
        public:
            // Loads TTF from font path. Color is specified here because changing SDL_Surface colors over and over is a pain.
            ttf(const char *fontPath, uint32_t fontColor);
            ttf(const std::string &fontPath, uint32_t fontColor);
            ~ttf();
            // Blits text string at X, Y with font size and color.
            void renderTextf(int x, int y, int fontSize, const char *format, ...);

        private:
            // Freetype face.
            FT_Face m_FontFace;
            // Font color
            uint32_t m_FontColor;
            // Vector buffer for font.
            std::vector<unsigned char> m_FontBuffer;
            // Whether font was successfuly initialized.
            bool m_IsInitialized = false;
            // Map of cached glyphs and data. Mapped according to size, char.
            std::map<std::pair<int, char>, glyphData> m_FontCacheMap;
            // Searches for, and loads glyph if not previously loaded.
            glyphData *getGlyph(char codepoint, int fontSize);
    };
} // namespace font
using ttfFont = std::shared_ptr<font::ttf>;
