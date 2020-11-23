#pragma once

#include "../game/cards.h"

namespace UNO { namespace Network {

using namespace Game;

enum class MsgType : uint8_t {
    JOIN_GAME,
    JOIN_GAME_RSP,
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

struct JoinGameRspMsg : public Msg {
    int mPlayerNum;
    // including player himself
    char mUsernames[];
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