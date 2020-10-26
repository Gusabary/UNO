#include "inputter.h"

namespace UNO { namespace UI {

InputAction Inputter::GetAction(int timeout)
{
    while (true) {
        // char ch = Common::Util::GetCharImmediately();
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
            case '\n': return InputAction::PLAY;
            case ' ':  return InputAction::PASS;
        }
    }
}

Game::CardColor Inputter::SpecifyNextColor(int timeout)
{
    while (true) {
        std::cout << "Specify the next color (R/Y/G/B): ";
        char ch;
        try {
            ch = Common::Util::GetCharWithTimeout(timeout, true);
        }
        catch (std::exception &e) {
            // timeout, red is default
            return Game::CardColor::RED;
        }

        // char ch = std::getchar();
        switch (ch) {
            case 'R': case 'r': return Game::CardColor::RED;
            case 'Y': case 'y': return Game::CardColor::YELLOW;
            case 'G': case 'g': return Game::CardColor::GREEN;
            case 'B': case 'b': return Game::CardColor::BLUE;
        }
    }
}

}}