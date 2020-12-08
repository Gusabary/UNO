#include "inputter.h"

namespace UNO { namespace UI {

InputAction Inputter::GetAction(int timeout)
{
    while (true) {
        char ch;
        try {
            ch = Common::Util::GetCharWithTimeout(timeout, true);
        }
        catch (std::exception &e) {
            // timeout
            return InputAction::PASS;
        }

        switch (ch) {
            case ',':  return InputAction::CURSOR_MOVE_LEFT;
            case '.':  return InputAction::CURSOR_MOVE_RIGHT;
#if defined(__unix__) || defined(__APPLE__)
            case '\n': return InputAction::PLAY;
#elif defined(_WIN32)
            case '\r': return InputAction::PLAY;
#endif
            case ' ':  return InputAction::PASS;
            case 'q': case 'Q': return InputAction::QUIT;
        }
    }
}

Game::CardColor Inputter::SpecifyNextColor(int timeout)
{
    while (true) {
        char ch;
        try {
            ch = Common::Util::GetCharWithTimeout(timeout, true);
        }
        catch (std::exception &e) {
            // timeout, red is default
            return Game::CardColor::RED;
        }

        switch (ch) {
            case 'R': case 'r': return Game::CardColor::RED;
            case 'Y': case 'y': return Game::CardColor::YELLOW;
            case 'G': case 'g': return Game::CardColor::GREEN;
            case 'B': case 'b': return Game::CardColor::BLUE;
        }
    }
}

}}