#include "msg.h"

namespace UNO { namespace Network {

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    std::string color;
    std::string text;

    switch (card.mColor) {
        case CardColor::RED:    color = "R"; break;
        case CardColor::YELLOW: color = "Y"; break;
        case CardColor::GREEN:  color = "G"; break;
        case CardColor::BLUE:   color = "B"; break;
        case CardColor::BLACK:  color = "";  break;
        default: assert(0);
    }

    switch (card.mText) {
        case CardText::ZERO:        text = "0";  break;
        case CardText::ONE:         text = "1";  break;
        case CardText::TWO:         text = "2";  break;
        case CardText::THREE:       text = "3";  break;
        case CardText::FOUR:        text = "4";  break;
        case CardText::FIVE:        text = "5";  break;
        case CardText::SIX:         text = "6";  break;
        case CardText::SEVEN:       text = "7";  break;
        case CardText::EIGHT:       text = "8";  break;
        case CardText::NINE:        text = "9";  break;
        case CardText::SKIP:        text = "S";  break;
        case CardText::REVERSE:     text = "R";  break;
        case CardText::DRAW_TWO:    text = "+2"; break;
        case CardText::WILD:        text = "W";  break;
        case CardText::DRAW_FOUR:   text = "+4"; break;
    }

    os << color << text;
    return os;
}
}}