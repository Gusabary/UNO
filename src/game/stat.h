#pragma once

#include "info.h"
#include "util.h"

namespace UNO { namespace Game {

using namespace Network;

class GameStat {
public:
    /// constructor for \c Player
    GameStat(const GameStartInfo &info);

    /// constructor for \c GameBoard
    GameStat(int firstPlayer, Card flippedCard);
    
    void NextPlayer(int playerNum);

    void UpdateAfterPlay(Card card);

    bool IsMyTurn() const { return mCurrentPlayer == 0; }

    int GetCurrentPlayer() const { return mCurrentPlayer; }

    bool IsInClockwise() const { return mIsInClockwise; }

    bool DoesGameEnd() const { return mGameEnds; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    int GetCardsNumToDraw() const { return mCardsNumToDraw; }

    void GameEnds() { mGameEnds = true; }

    void Reverse() { mIsInClockwise = !mIsInClockwise; }

private:
    int mCurrentPlayer;
    bool mIsInClockwise;
    bool mGameEnds{false};

    // currently the two fields below are not used by GameStat of GameBoard
    Card mLastPlayedCard;
    int mCardsNumToDraw{1};  // +2 and +4 can accumulate
};

class PlayerStat {
public:
    PlayerStat() {}
    explicit PlayerStat(const std::string &username, int remainingHandCardsNum);

    void UpdateAfterDraw(int number);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

    std::string GetUsername() const { return mUsername; }

    int GetRemainingHandCardsNum() const { return mRemainingHandCardsNum;  }

    bool GetDoPlayInLastRound() const { return mDoPlayInLastRound; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

private:
    const std::string mUsername;

    int mRemainingHandCardsNum;
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};
    
    friend std::ostream& operator<<(std::ostream& os, const PlayerStat& stat);
};

}}