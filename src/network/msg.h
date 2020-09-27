#pragma once

#include <iostream>
#include <cassert>

namespace UNO { namespace Network {

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

    Card() {}
    Card(CardColor color, CardText text) : mColor(color), mText(text) {}

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};

enum class MsgType : uint8_t {
    JOIN_GAME,
    GAME_START,
    DRAW,
    DRAW_RSP,
    SKIPPED,
    PLAY
};

struct Msg {
    MsgType mType;
    int mLen;  // **not** include the mType and mLen itself
};

struct JoinGameMsg : public Msg {
    char mUsername[];
};

struct GameStartMsg : public Msg {
    Card mInitHandCards[7];
    Card mFlippedCard;  // indicating the first card that should be played
    int mFirstPlayer;  // the index of the first player to play a card
    // usernames of all players, not including player himself, ' ' as delimiter
    // and the order is from left side of the player to right side
    char mUsernames[];
};

struct DrawMsg : public Msg {
    int mNumber;  // the number of cards to draw
};

struct DrawRspMsg : public Msg {
    int mNumber;
    Card mCards[];
};

struct SkippedMsg : public Msg {
};

struct PlayMsg : public Msg {
    Card mCard;
    CardColor mNextColor;  // valid only if mCard is black
};
}}