#include "inputter.h"

namespace UNO { namespace UI {

InputAction Inputter::GetAction()
{
    while (true) {
        char ch = Common::Util::GetCharImmediately();
        switch (ch) {
            case ',':  return InputAction::CURSOR_MOVE_LEFT;
            case '.':  return InputAction::CURSOR_MOVE_RIGHT;
            case '\r': return InputAction::PLAY;
            case ' ':  return InputAction::PASS;
        }
    }
}

Game::CardColor Inputter::SpecifyNextColor()
{
    while (true) {
        std::cout << "Specify the next color (R/Y/G/B): ";
        char ch = std::getchar();
        switch (ch) {
            case 'R': case 'r': return Game::CardColor::RED;
            case 'Y': case 'y': return Game::CardColor::YELLOW;
            case 'G': case 'g': return Game::CardColor::GREEN;
            case 'B': case 'b': return Game::CardColor::BLUE;
        }
    }
}

}}