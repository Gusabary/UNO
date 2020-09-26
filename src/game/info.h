#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <algorithm>

#include "../network/msg.h"

namespace UNO { namespace Game {

using namespace Network;

struct Info
{};

struct JoinGameInfo : public Info {
    std::string mUsername;

    JoinGameInfo() {}
    JoinGameInfo(const std::string &username) : mUsername(username) {}

    void Serialize(uint8_t *buffer) const;
    static JoinGameInfo Deserialize(const uint8_t *buffer);
};

struct GameStartInfo : public Info {
    std::array<Card, 7> mInitHandCards;
    Card mFlippedCard;
    int mFirstPlayer;
    std::vector<std::string> mUsernames;

    void Serialize(uint8_t *buffer) const;
    static GameStartInfo Deserialize(const uint8_t *buffer);
};
}}