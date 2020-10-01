#pragma once

#include <iostream>
#include <cassert>

namespace UNO { namespace Network {

enum class CardColor : uint8_t {
    RED, YELLOW, GREEN, BLUE, BLACK
};
std::ostream& operator<<(std::ostream& os, const CardColor& color);

enum class CardText : uint8_t {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, 
    SKIP, REVERSE, DRAW_TWO, WILD, DRAW_FOUR, EMPTY
    /**
     * EMPTY is for conditions like:
     *   1. in the game start, the flipped card is yellow +2,
     *      now for the first player, the last played card is `yellow empty`,
     *      which means he can only play a yellow or wild card
     *   2. player A played a yellow +2 / +4, player B got the draw,
     *      now for player C, the last played card is `yellow empty`
     */
};

struct Card {
    CardColor mColor;
    CardText mText;

    Card() {}
    Card(const char *str);
    Card(CardColor color, CardText text) : mColor(color), mText(text) {}

    static CardColor ConvertFromCharToColor(char c);

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};
inline bool operator==(const Card &lhs, const Card &rhs) {
    return (lhs.mColor == rhs.mColor) && (lhs.mText == rhs.mText);
}

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