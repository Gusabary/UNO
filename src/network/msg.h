#pragma once

#include <iostream>
#include <cassert>

namespace UNO { namespace Network {

enum class CardColor : uint8_t {
    RED, YELLOW, GREEN, BLUE, BLACK
};
std::ostream& operator<<(std::ostream& os, const CardColor& color);

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
    ACTION,
    DRAW_RSP,
    GAME_END
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

enum class ActionType : uint8_t {
    DRAW,
    SKIP,
    PLAY
};
std::ostream& operator<<(std::ostream& os, const ActionType& type);

struct ActionMsg : public Msg {
    ActionType mActionType;
    int mPlayerIndex;
};

struct DrawMsg : public ActionMsg {
    int mNumber;  // the number of cards to draw
};

struct SkipMsg : public ActionMsg {
};

struct PlayMsg : public ActionMsg {
    Card mCard;
    CardColor mNextColor;  // valid only if mCard is black
};

struct DrawRspMsg : public Msg {
    int mNumber;
    Card mCards[];
};

struct GameEndMsg : public Msg {
    int mWinner;
};
}}