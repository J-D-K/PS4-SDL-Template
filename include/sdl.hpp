#pragma once
#include <SDL2/SDL.h>

namespace SDL
{
    // Initializes SDL2
    bool initialize(void);
    // Quits SDL2. Not really useful since PS4 hates breaking and returning the main loop?
    void exit(void);
    // Returns SDL_Renderer.
    SDL_Renderer *getRenderer(void);
    // Begins a frame. Clears screen to black.
    void frameBegin(void);
    // Ends a frame, flips to screen. Locked to 60fps.
    void frameEnd(void);
} // namespace SDL
