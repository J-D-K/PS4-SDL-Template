#include "font.hpp"
#include "assetmanager.hpp"
#include "log.hpp"
#include "sdl.hpp"
#include <array>
#include <cstdarg>
#include <fstream>
#include <orbis/Sysmodule.h>

namespace
{
    // Freetype lib.
    FT_Library s_FTLib;
    // SDL_Surface masks.
    constexpr uint32_t RED_MASK = 0xFF000000;
    constexpr uint32_t GREEN_MASK = 0x00FF0000;
    constexpr uint32_t BLUE_MASK = 0x0000FF00;
    constexpr uint32_t ALPHA_MASK = 0x000000FF;
    // Va buffer size for renderTextf
    constexpr int VA_BUFFER_SIZE = 0x1000;
} // namespace

bool font::initialize(void)
{
    // Try to load sysmodule first before continuing.
    int sceError = sceSysmoduleLoadModule(0x009A);
    if (sceError < 0)
    {
        return false;
    }

    // Init freetype.
    int ftError = FT_Init_FreeType(&s_FTLib);
    if (ftError != 0)
    {
        return false;
    }
    return true;
}

void font::exit(void)
{
    FT_Done_FreeType(s_FTLib);
    sceSysmoduleUnloadModule(0x009A);
}

font::ttf::ttf(const char *fontPath, uint32_t fontColor) : m_FontColor(fontColor & 0xFFFFFF00)
{
    // Open font file at the end.
    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (fontFile.is_open() == false)
    {
        return;
    }
    // Save size.
    size_t fontSize = fontFile.tellg();

    fontFile.seekg(0, std::ios::beg);
    // Resize to load font to memory.
    m_FontBuffer.resize(fontSize);
    fontFile.read(reinterpret_cast<char *>(m_FontBuffer.data()), fontSize);
    // Create memory face
    int ftError = FT_New_Memory_Face(s_FTLib, reinterpret_cast<FT_Byte *>(m_FontBuffer.data()), fontSize, 0, &m_FontFace);
    if (ftError != 0)
    {
        return;
    }
    // This is a safety measure.
    m_IsInitialized = true;
}

// This just calls the first constructor using the c_str() of fontPath
font::ttf::ttf(const std::string &fontPath, uint32_t fontColor) : ttf(fontPath.c_str(), fontColor)
{
}

font::ttf::~ttf()
{
    FT_Done_Face(m_FontFace);
}

void font::ttf::renderTextf(int x, int y, int fontSize, const char *format, ...)
{
    if (m_IsInitialized == false)
    {
        return;
    }
    // Resize to fontSize in pixels high
    FT_Set_Pixel_Sizes(m_FontFace, 0, fontSize);
    // Va
    std::array<char, VA_BUFFER_SIZE> vaBuffer = {0};
    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);
    // Need to save original x coordinate.
    int originalX = x;

    size_t stringLength = std::strlen(vaBuffer.data());
    for (int i = 0; i < stringLength; i++)
    {
        if (vaBuffer.at(i) == '\n')
        {
            y += fontSize + (fontSize / 3);
            x = originalX;
            continue;
        }

        font::glyphData *currentGlyph = getGlyph(vaBuffer.at(i), fontSize);
        // Space needs extra handling here. The texture is 0x0, which causes issues with the software rendering.
        if (currentGlyph != NULL && vaBuffer.at(i) != ' ')
        {
            currentGlyph->glyphTexture->renderAt(x + currentGlyph->left, y + (fontSize - currentGlyph->top));
            x += currentGlyph->advanceX;
        }
        else if (currentGlyph != NULL && vaBuffer.at(i) == ' ')
        {
            x += currentGlyph->advanceX;
        }
    }
}

font::glyphData *font::ttf::getGlyph(char codepoint, int fontSize)
{
    // If it's already loaded and rendered with fontSize, just return reference to it.
    if (m_FontCacheMap.find(std::make_pair(fontSize, codepoint)) != m_FontCacheMap.end())
    {
        return &m_FontCacheMap.at(std::make_pair(fontSize, codepoint));
    }
    // Load glyph with Freetype.
    FT_UInt charIndex = FT_Get_Char_Index(m_FontFace, codepoint);
    int ftError = FT_Load_Glyph(m_FontFace, charIndex, FT_LOAD_RENDER);
    if (charIndex == 0 || ftError != 0)
    {
        return NULL;
    }
    // Pointers that make it easier to write and read.
    FT_GlyphSlot glyphSlot = m_FontFace->glyph;
    FT_Bitmap glyphBitmap = glyphSlot->bitmap;
    // Just to be sure. This usually never fails.
    if (glyphBitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
    {
        return NULL;
    }
    // This is a temporary surface to create a texture from. Though, given this is all software rendered, I guess it doesn't matter?
    SDL_Surface *glyphSurface = SDL_CreateRGBSurface(0, glyphBitmap.width, glyphBitmap.rows, 32, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
    if (glyphSurface == NULL)
    {
        return NULL;
    }
    // Size of bitmap for loop. Surface should be same just uint32_t AKA RGBA
    size_t bitmapSize = glyphBitmap.width * glyphBitmap.rows;
    // Pointers to glyph bitmap pixels and surface pixels.
    uint8_t *bitmapPixels = reinterpret_cast<uint8_t *>(glyphBitmap.buffer);
    uint32_t *surfacePixels = reinterpret_cast<uint32_t *>(glyphSurface->pixels);
    // Loop and fill surface with pixels.
    for (size_t i = 0; i < bitmapSize; i++)
    {
        *surfacePixels++ = m_FontColor | *bitmapPixels++;
    }
    // Convert to texture and free surface
    // Manager needs identifying string.
    std::string glyphName = std::to_string(fontSize) + "-" + std::to_string(codepoint);
    SDL::sdlTexture glyphTexture = texturemanager::createAndLoadAsset(glyphName, glyphSurface);
    SDL_FreeSurface(glyphSurface);
    // Add to cache map.
    m_FontCacheMap[std::make_pair(fontSize, codepoint)] = {.left = glyphSlot->bitmap_left,
                                                           .top = glyphSlot->bitmap_top,
                                                           .advanceX = static_cast<int>(glyphSlot->advance.x >> 6),
                                                           .glyphTexture = glyphTexture};

    return &m_FontCacheMap.at(std::make_pair(fontSize, codepoint));
}
