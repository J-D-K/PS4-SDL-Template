#include "input.hpp"
#include "log.hpp"
#include <cstddef>
#include <orbis/UserService.h>

// This is just setup for a single controller. Who multiplays a homebrew app anyway?
namespace
{
    // Controller handle.
    int s_ControllerHandle;
    // Pad struct.
    OrbisPadData s_GamePad;
    // Previous button state for checking
    unsigned int s_PreviousPadState = 0;
} // namespace

bool input::initialize(void)
{
    int sceError = scePadInit();
    if (sceError < 0)
    {
        logger::log("Error initializing pad library.");
        return false;
    }

    // I guess this is needed to get the user ID?
    int userID = 0;
    OrbisUserServiceInitializeParams initParams = {.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST};
    sceError = sceUserServiceInitialize(&initParams);
    if (sceError < 0)
    {
        logger::log("Error 0x%X initializing user service.", static_cast<unsigned int>(sceError));
        return false;
    }

    sceError = sceUserServiceGetInitialUser(&userID);
    if (sceError < 0)
    {
        logger::log("Error getting initial user.");
        return false;
    }

    s_ControllerHandle = scePadOpen(userID, ORBIS_PAD_PORT_TYPE_STANDARD, 0, NULL);
    if (s_ControllerHandle < 0)
    {
        logger::log("Error opening pad.");
        return false;
    }

    return true;
}

void input::exit(void)
{
    scePadClose(s_ControllerHandle);
}

void input::update(void)
{
    // Record previous state
    s_PreviousPadState = s_GamePad.buttons;
    // Update current state. Error checking this is kind of pointless with how I'm writing this.
    scePadReadState(s_ControllerHandle, &s_GamePad);
}

bool input::buttonIsPressed(unsigned int button)
{
    if (!(s_PreviousPadState & button) && (s_GamePad.buttons & button))
    {
        return true;
    }
    return false;
}

bool input::buttonIsHeld(unsigned int button)
{
    if ((s_PreviousPadState & button) && (s_GamePad.buttons & button))
    {
        return true;
    }
    return false;
}

bool input::buttonIsReleased(unsigned int button)
{
    if ((s_PreviousPadState & button) && !(s_GamePad.buttons & button))
    {
        return true;
    }
    return false;
}
