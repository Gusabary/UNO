#pragma once

#include <iostream>
#include <array>
#include <vector>

namespace UNO { namespace Game {

enum class CardColor : uint8_t {
    RED, YELLOW, GREEN, BLUE, BLACK
};

enum class CardText : uint8_t {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, 
    EIGHT, NINE, SKIP, REVERSE, DRAW_TWO, WILD, DRAW_FOUR
};

struct Card {
    CardColor mColor;
    CardText mText;
};

struct Info
{};

struct JoinGameInfo : public Info {
    std::string mUsername;

    JoinGameInfo(const char *username) : mUsername(username) {}
    JoinGameInfo(const std::string &username) : mUsername(username) {}
};

struct GameStartInfo : public Info {
    std::array<Card, 7> mInitHandCards;
    Card mFlippedCard;
    int mFirstPlayer;
    std::vector<std::string> mUsernames;
};
}}