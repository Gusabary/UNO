#pragma once

#include <iostream>

#include "../game/info.h"

namespace UNO { namespace Network {

using namespace Game;

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

    Msg(MsgType type, int len) : mType(type), mLen(len) {}
};

struct JoinGameMsg : public Msg {
    char mUsername[];

    void FromInfo(const JoinGameInfo &info) {
        mType = MsgType::JOIN_GAME;
        mLen = info.mUsername.size();  // XXX: need +1? consider the terminated null?
        std::strcpy(mUsername, info.mUsername.c_str());
    }

    JoinGameInfo ToInfo() {
        return JoinGameInfo(mUsername);
    }
};

struct GameStartMsg : public Msg {
    Card mInitHandCards[7];
    Card mFlippedCard;  // indicating the first card that should be played
    int mFirstPlayer;  // the index of the first player to play a card
    // usernames of all players, not including player himself, ' ' as delimiter
    // and the order is from left side of the player to right side
    char mUsernames[];

    void FromInfo(const GameStartInfo &info) {
        mType = MsgType::GAME_START;

        std::string usernames{};
        std::for_each(info.mUsernames.begin(), info.mUsernames.end(),
            [&usernames](const std::string &username) {
                // ' ' as delimiter of usernames
                usernames.append(username).push_back(' ');
            }
        );
        mLen = sizeof(Card) * 8 + sizeof(int) + usernames.size();

        std::copy(info.mInitHandCards.begin(), info.mInitHandCards.end(), mInitHandCards);
        mFlippedCard = info.mFlippedCard;
        mFirstPlayer = info.mFirstPlayer;
        std::strcpy(mUsernames, usernames.c_str());
    }

    GameStartInfo ToInfo() {
        GameStartInfo info;
        std::copy(std::begin(mInitHandCards), std::end(mInitHandCards),
                  info.mInitHandCards.begin());
        info.mFlippedCard = mFlippedCard;
        info.mFirstPlayer = mFirstPlayer;

        std::string usernames(mUsernames);
        while (!usernames.empty()) {
            int pos = usernames.find(' ');
            info.mUsernames.emplace_back(usernames, pos);
            usernames.erase(0, pos + 1);
        }

        return info;
    }
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