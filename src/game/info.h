#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <algorithm>
#include <memory>

#include "../network/msg.h"

namespace UNO { namespace Game {

using namespace Network;

struct Info {
    // enable polymorphism
    virtual ~Info() {}
};

struct JoinGameInfo : public Info {
    std::string mUsername;

    JoinGameInfo() {}
    JoinGameInfo(const std::string &username) : mUsername(username) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<JoinGameInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const JoinGameInfo &info) const;
    friend std::ostream &operator<<(std::ostream &os, const JoinGameInfo &info);
};

struct JoinGameRspInfo : public Info {
    int mPlayerNum;
    std::vector<std::string> mUsernames;

    JoinGameRspInfo() {}
    JoinGameRspInfo(int playerNum, const std::vector<std::string> &usernames)
        : mPlayerNum(playerNum), mUsernames(usernames) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<JoinGameRspInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const JoinGameRspInfo &info) const;
    friend std::ostream &operator<<(std::ostream &os, const JoinGameRspInfo &info);
};

struct GameStartInfo : public Info {
    std::array<Card, 7> mInitHandCards;
    Card mFlippedCard;
    int mFirstPlayer;
    std::vector<std::string> mUsernames;

    GameStartInfo() {}
    GameStartInfo(const std::array<Card, 7> &initHandCards,
        Card flippedCard, int firstPlayer,
        const std::vector<std::string> &usernames) 
        : mInitHandCards(initHandCards), mFlippedCard(flippedCard),
        mFirstPlayer(firstPlayer), mUsernames(usernames) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<GameStartInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const GameStartInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const GameStartInfo& info);
};

struct ActionInfo : public Info {
    ActionType mActionType;
    int mPlayerIndex{-1};

    ActionInfo() {}
    ActionInfo(ActionType actionType) : mActionType(actionType) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<ActionInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const ActionInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const ActionInfo& info);

    // enable polymorphism
    virtual ~ActionInfo() {}
};

struct DrawInfo : public ActionInfo {
    int mNumber;

    DrawInfo() : ActionInfo(ActionType::DRAW) {}
    DrawInfo(int number) : ActionInfo(ActionType::DRAW), mNumber(number) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<DrawInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const DrawInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const DrawInfo& info);
};

struct SkipInfo : public ActionInfo {
    SkipInfo() : ActionInfo(ActionType::SKIP) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<SkipInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const SkipInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const SkipInfo& info);
};

struct PlayInfo : public ActionInfo {
    Card mCard;
    CardColor mNextColor;

    PlayInfo() : ActionInfo(ActionType::PLAY) {}
    PlayInfo(Card card) : PlayInfo(card, card.mColor) {}
    PlayInfo(Card card, CardColor nextColor)
        : ActionInfo(ActionType::PLAY), mCard(card), mNextColor(nextColor) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<PlayInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const PlayInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const PlayInfo& info);
};

struct DrawRspInfo : public Info {
    int mNumber;
    std::vector<Card> mCards;

    DrawRspInfo() {}
    DrawRspInfo(int number, const std::vector<Card> &cards) 
        : mNumber(number), mCards(cards) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<DrawRspInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const DrawRspInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const DrawRspInfo& info);
};

struct GameEndInfo : public Info {
    int mWinner;

    GameEndInfo() {}
    GameEndInfo(int winner) : mWinner(winner) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<GameEndInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const GameEndInfo &info) const;
};
}}