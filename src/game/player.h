#pragma once

#include <memory>

#include "../network/client.h"

namespace UNO { namespace Game {

using namespace Network;

struct PlayerStat {
    const std::string mUsername;
    int mRemainingHandCardsNum;
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};

    PlayerStat() {}
    PlayerStat(const std::string &username, int remainingHandCardsNum)
        : mUsername(username), mRemainingHandCardsNum(remainingHandCardsNum)
    {}
    
    friend std::ostream& operator<<(std::ostream& os, const PlayerStat& stat);
};

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

    void UpdateStateAfterPlaying(Card cardPlayed);

    int WrapWithPlayerNum(int numToWrap);

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

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}