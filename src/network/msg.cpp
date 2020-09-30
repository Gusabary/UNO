#include "msg.h"

namespace UNO { namespace Network {

Card::Card(const char *str)
{
    switch (*str) {
        case 'R': mColor = CardColor::RED;    str++; break;
        case 'Y': mColor = CardColor::YELLOW; str++; break;
        case 'G': mColor = CardColor::GREEN;  str++; break;
        case 'B': mColor = CardColor::BLUE;   str++; break;
        default:  mColor = CardColor::BLACK;
    }

    switch (*str) {
        case '0': mText = CardText::ZERO;    break;
        case '1': mText = CardText::ONE;     break;
        case '2': mText = CardText::TWO;     break;
        case '3': mText = CardText::THREE;   break;
        case '4': mText = CardText::FOUR;    break;
        case '5': mText = CardText::FIVE;    break;
        case '6': mText = CardText::SIX;     break;
        case '7': mText = CardText::SEVEN;   break;
        case '8': mText = CardText::EIGHT;   break;
        case '9': mText = CardText::NINE;    break;
        case 'S': mText = CardText::SKIP;    break;
        case 'R': mText = CardText::REVERSE; break;
        case 'W': mText = CardText::WILD;    break;
        case '+': mText = (*(str + 1) == '2') ? 
            CardText::DRAW_TWO : CardText::DRAW_FOUR; 
            break;
        default: assert(0);
    }
}

CardColor Card::ConvertFromCharToColor(char c)
{
    switch (c) {
        case 'R': return CardColor::RED;
        case 'Y': return CardColor::YELLOW;
        case 'G': return CardColor::GREEN;
        case 'B': return CardColor::BLUE;
    }
    assert(0);
}

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
        default: assert(0);
    }

    os << color << text;
    return os;
}

std::ostream& operator<<(std::ostream& os, const CardColor& color)
{
    std::string colorStr;
    switch (color) {
        case CardColor::RED:    colorStr = "RED";    break;
        case CardColor::YELLOW: colorStr = "YELLOW"; break;
        case CardColor::GREEN:  colorStr = "GREEN";  break;
        case CardColor::BLUE:   colorStr = "BLUE";   break;
        default: assert(0);
    }

    os << colorStr;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ActionType& type)
{
    std::string typeStr;
    switch (type) {
        case ActionType::DRAW: typeStr = "DRAW"; break;
        case ActionType::SKIP: typeStr = "SKIP"; break;
        case ActionType::PLAY: typeStr = "PLAY"; break;
        default: assert(0);
    }

    os << typeStr;
    return os;
}
}}