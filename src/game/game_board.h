#pragma once

#include <memory>
#include <random>
#include <deque>
#include <cstdlib>

#include "../network/server.h"

namespace UNO { namespace Game {

using namespace Network;

class GameBoard {
public:
    explicit GameBoard(std::string port);

private:
    void ReceiveUsername(int index, const std::string &username);

    void StartGame();

    void InitDeck();
    std::vector<std::array<Card, 7>> DealInitHandCards();

    int WrapWithPlayerNum(int numToWrap, int player);

private:
    constexpr static int PLAYER_NUM = 3;
    Network::Server mServer;

    std::deque<Card> mDeck;
    std::deque<Card> mDiscardPile;

    std::vector<std::string> mUsernames;
};
}}