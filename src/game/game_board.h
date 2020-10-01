#pragma once

#include <memory>
#include <random>
#include <deque>
#include <cstdlib>

#include "player_stat.h"
#include "../network/server.h"

namespace UNO { namespace Game {

using namespace Network;

class GameBoard {
public:
    explicit GameBoard(std::string port);

private:
    void ReceiveUsername(int index, const std::string &username);

    void StartGame();

    int WrapWithPlayerNum(int numToWrap);

    void GameLoop();
    
    void HandleDraw(std::unique_ptr<DrawInfo> info);
    
    void HandleSkip(std::unique_ptr<SkipInfo> info);
    
    void HandlePlay(std::unique_ptr<PlayInfo> info);

    void Win();

private:
    constexpr static int PLAYER_NUM = 3;
    Network::Server mServer;

    // state of game board
    std::unique_ptr<Deck> mDeck;
    std::unique_ptr<DiscardPile> mDiscardPile;
    int mCurrentPlayer;
    bool mIsInClockwise;
    bool mGameEnds{false};

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}