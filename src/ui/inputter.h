#pragma once

#include <string>
#include <iostream>

#include "../common/util.h"
#include "../game/cards.h"

namespace UNO { namespace UI {

enum class InputAction : uint8_t {
    CURSOR_MOVE_LEFT,
    CURSOR_MOVE_RIGHT,
    PLAY,
    PASS,  // skip or draw
    QUIT
};

class Inputter {
public:
    Inputter() {}

    /**
     * Wait for player to input an action with a time limit \param timeout.
     */
    InputAction GetAction(int timeout);

    /**
     * After the player plays a 'W' or '+4', he needs to specify the next color to turn to.
     */
    Game::CardColor SpecifyNextColor(int timeout);

private:
};

}}