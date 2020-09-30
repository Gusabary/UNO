#pragma once

#include <memory>
#include <set>

#include "player_stat.h"
#include "../network/client.h"

namespace UNO { namespace Game {

using namespace Network;

class Player {
public:
    explicit Player(std::string username, std::string host, std::string port);

private:
    void JoinGame();

    void GameLoop();

    void HandleDraw(const std::unique_ptr<DrawInfo> &info);
    
    void HandleSkip(const std::unique_ptr<SkipInfo> &info);
    
    void HandlePlay(const std::unique_ptr<PlayInfo> &info);

    bool CanCardBePlayed(Card cardToPlay);

    void UpdateStateAfterPlay(int playerIndex, Card cardPlayed);

    int WrapWithPlayerNum(int numToWrap);

    void Win(int playerIndex);

    void PrintLocalState();

private:
    const std::string mUsername;
    Network::Client mClient;

    std::vector<Card> mHandCards;

    // state of game board
    Card mLastPlayedCard;
    int mCurrentPlayer;
    bool mIsInClockwise;
    int mCardsNumToDraw;  // +2 and +4 can accumulate
    bool mGameEnds{false};

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}