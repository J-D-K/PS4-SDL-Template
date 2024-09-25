#include "texture.hpp"
#include "sdl.hpp"

SDL::texture::texture(int width, int height, int sdlTextureFlags) : m_Width(width), m_Height(height)
{
    m_Texture = SDL_CreateTexture(SDL::getRenderer(), SDL_PIXELFORMAT_RGBA8888, sdlTextureFlags, width, height);
}

SDL::texture::texture(SDL_Surface *surface) : m_Width(surface->w), m_Height(surface->h)
{
    m_Texture = SDL_CreateTextureFromSurface(SDL::getRenderer(), surface);
}

SDL::texture::~texture()
{
    SDL_DestroyTexture(m_Texture);
}

void SDL::texture::renderAt(int x, int y)
{
    SDL_Rect sourceRect = {0, 0, m_Width, m_Height};
    SDL_Rect destinationRect = {x, y, m_Width, m_Height};
    SDL_RenderCopy(SDL::getRenderer(), m_Texture, &sourceRect, &destinationRect);
}
