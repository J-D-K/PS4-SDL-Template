#include "sdl.hpp"
#include "log.hpp"
#include <SDL2/SDL.h>
#include <orbis/VideoOut.h>

namespace
{
    // For storing system's resolution.
    uint32_t s_ScreenWidth = 0;
    uint32_t s_ScreenHeight = 0;
    // Window & surface.
    SDL_Window *s_Window = NULL;
    SDL_Surface *s_WindowSurface = NULL;
    // Renderer
    SDL_Renderer *s_Renderer = NULL;
    // For FPS capping
    constexpr int FPS_CAP = 1000 / 60;
    int s_FrameBeginTicks = 0;
    int s_FrameEndTicks = 0;
} // namespace

bool SDL::initialize(void)
{
    // This feels stupid, but we need to open a video handle to detect resolution and then close it right away.
    int videoHandle = sceVideoOutOpen(ORBIS_VIDEO_USER_MAIN, ORBIS_VIDEO_OUT_BUS_MAIN, 0, 0);
    if (videoHandle < 0)
    {
        logger::log("sceVideoOutOpen failed.");
        return false;
    }

    OrbisVideoOutResolutionStatus videoResolution;
    int sceError = sceVideoOutGetResolutionStatus(videoHandle, &videoResolution);
    if (sceError < 0)
    {
        logger::log("sceVideoOutGetResolutionStatus failed.");
        return false;
    }
    // Close the handle and save the width and height of the screen.
    sceVideoOutClose(videoHandle);
    s_ScreenWidth = videoResolution.width;
    s_ScreenHeight = videoResolution.height;

    logger::log("Video resolution %i/%i", videoResolution.width, videoResolution.height);

    // Try to init SDL
    int sdlError = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (sdlError != 0)
    {
        logger::log("SDL_Init failed.");
        return false;
    }

    // Try to create window.
    s_Window =
        SDL_CreateWindow("PS4_SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_FULLSCREEN);
    if (s_Window == NULL)
    {
        logger::log("SDL_CreateWindow failed.");
        return false;
    }

    s_Renderer = SDL_CreateRenderer(s_Window, -1, SDL_RENDERER_SOFTWARE);
    if (s_Renderer == NULL)
    {
        logger::log("SDL_CreateRenderer failed.");
        return false;
    }

    return true;
}

void SDL::exit(void)
{
    SDL_DestroyWindow(s_Window);
    SDL_Quit();
}

SDL_Renderer *SDL::getRenderer(void)
{
    return s_Renderer;
}

void SDL::frameBegin(void)
{
    // Clear framebuffer to solid black.
    SDL_SetRenderDrawColor(s_Renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(s_Renderer);
    // Get starting ticks.
    s_FrameBeginTicks = SDL_GetTicks();
}

void SDL::frameEnd(void)
{
    // Flip screen.
    SDL_RenderPresent(s_Renderer);
    // Get ticks
    s_FrameEndTicks = SDL_GetTicks();
    // Delay based upon whether we're too fast to begin next frame, more or less.
    if (FPS_CAP > s_FrameEndTicks - s_FrameBeginTicks)
    {
        SDL_Delay(s_FrameEndTicks - s_FrameBeginTicks);
    }
}
