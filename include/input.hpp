#pragma once
#include <orbis/Pad.h>

namespace input
{
    // Initializes sce controller input.
    bool initialize(void);
    void exit(void);
    // Updates pad state
    void update(void);
    // Checks if button is [insert state here]
    bool buttonIsPressed(unsigned int button);
    bool buttonIsHeld(unsigned int button);
    bool buttonIsReleased(unsigned int button);
} // namespace input
