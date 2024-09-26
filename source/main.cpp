#include "assetmanager.hpp"
#include "color.h"
#include "font.hpp"
#include "input.hpp"
#include "log.hpp"
#include "sdl.hpp"
#include <cstdlib>
#include <ctime>

uint8_t generateRandomValue(void)
{
    return rand() % 0xFF;
}

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

    if (input::initialize() == false)
    {
        return -3;
    }


    // Font testing using free google font.
    ttfFont font = fontmanager::createAndLoadAsset("font", "/app0/assets/fonts/font.ttf");

    // Randomized color when buttons are pressed for testing.
    uint8_t red = 0xFF;
    uint8_t green = 0xFF;
    uint8_t blue = 0xFF;
    while (true)
    {
        input::update();

        // Up is red, left is green, right is blue. These are unsigned so they'll roll over after 0xFF.
        if (input::buttonIsHeld(ORBIS_PAD_BUTTON_UP))
        {
            red += 5;
        }
        else if (input::buttonIsHeld(ORBIS_PAD_BUTTON_LEFT))
        {
            green += 5;
        }
        else if (input::buttonIsHeld(ORBIS_PAD_BUTTON_RIGHT))
        {
            blue += 5;
        }

        SDL::frameBegin();
        font->renderTextf(32, 32, 64, CREATE_COLOR(red, green, blue, 0xFF), "HELLO PS4 WOOOOOOO");
        SDL::frameEnd();
    }

    // This stuff in never truly reached, but I'm used to doing it so...
    input::exit();
    font::exit();
    SDL::exit();
    return 0;
}
