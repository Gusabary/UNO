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
    PASS  // skip or draw
};

class Inputter {
public:
    Inputter() {}

    InputAction GetAction();

    Game::CardColor SpecifyNextColor();

private:
};

}}