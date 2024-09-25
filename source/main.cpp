#include "assetmanager.hpp"
#include "font.hpp"
#include "log.hpp"
#include "sdl.hpp"

int main(void)
{
    logger::initialize();
    // PS4 hates this.
    if (SDL::initialize() == false)
    {
        return -1;
    }

    if (font::initialize() == false)
    {
        return -2;
    }

    // Font testing using free google font.
    ttfFont font = fontmanager::createAndLoadAsset("roboto", "/app0/assets/fonts/font.ttf", 0xFFFFFFFF);

    while (true)
    {
        SDL::frameBegin();
        font->renderTextf(8, 8, 24, "Hello, PS4.");
        SDL::frameEnd();
    }

    // This stuff in never truly reached, but I'm used to doing it so...
    font::exit();
    SDL::exit();
    return 0;
}
