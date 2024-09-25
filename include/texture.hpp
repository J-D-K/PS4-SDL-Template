#pragma once
#include <SDL2/SDL.h>
#include <memory>

namespace SDL
{
    class texture
    {
        public:
            texture(void) = default;
            texture(int width, int height, int sdlTextureFlags);
            // Creates a texture using surface.
            texture(SDL_Surface *surface);
            ~texture();
            // Renders texture as-is at x, y.
            void renderAt(int x, int y);

        private:
            // Texture.
            SDL_Texture *m_Texture = NULL;
            // Width and height of texture.
            int m_Width = 0;
            int m_Height = 0;
    };
    // Mostly for laziness.
    using sdlTexture = std::shared_ptr<SDL::texture>;
} // namespace SDL
