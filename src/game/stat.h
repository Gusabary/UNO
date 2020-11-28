#pragma once

#include "info.h"
#include "../common/util.h"

namespace UNO { namespace Game {

using namespace Network;

class GameStat {
public:
    /// constructor for \c Player
    GameStat(const GameStartInfo &info);

    /// constructor for \c GameBoard
    GameStat(int firstPlayer, Card flippedCard);
    
    void NextPlayer();

    void UpdateAfterDraw();

    void UpdateAfterSkip();

    void UpdateAfterPlay(Card card);

    void Tick();

    bool IsMyTurn() const { return mCurrentPlayer == 0; }

    bool IsSkipped() const { return mLastPlayedCard.mText == CardText::SKIP; }

    int GetCurrentPlayer() const { return mCurrentPlayer; }

    bool IsInClockwise() const { return mIsInClockwise; }

    bool DoesGameEnd() const { return mGameEnds; }

    int GetTimeElapsed() const { return mTimeElapsed; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    int GetCardsNumToDraw() const { return mCardsNumToDraw; }

    void GameEnds() { mGameEnds = true; mCurrentPlayer = -1; }

    void Reverse() { mIsInClockwise = !mIsInClockwise; }

    // for tests
    void SetCurrentPlayer(int currentPlayer) { mCurrentPlayer = currentPlayer; }
    
    void SetIsInClockwise(bool isInClockwise) { mIsInClockwise = isInClockwise; }

    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }

    void SetCardsNumToDraw(int cardsNumToDraw) { mCardsNumToDraw = cardsNumToDraw; }

private:
    int mCurrentPlayer;
    bool mIsInClockwise;
    bool mGameEnds{false};
    int mTimeElapsed{0};

    // currently the two fields below are not used by GameStat of GameBoard
    Card mLastPlayedCard{};
    int mCardsNumToDraw{1};  // +2 and +4 can accumulate
};

class PlayerStat {
public:
    PlayerStat() {}
    explicit PlayerStat(const std::string &username, int remainingHandCardsNum);

    void UpdateAfterDraw(int number, int indexOfNewlyDrawn = -1);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

    std::string GetUsername() const { return mUsername; }

    int GetRemainingHandCardsNum() const { return mRemainingHandCardsNum;  }

    bool DoPlayInLastRound() const { return mDoPlayInLastRound; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    bool HasChanceToPlayAfterDraw() const { return mHasChanceToPlayAfterDraw; }

    int GetIndexOfNewlyDrawn() const { return mIndexOfNewlyDrawn; }

    // for test
    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }

private:
    const std::string mUsername;

    int mRemainingHandCardsNum;
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};
    bool mHasChanceToPlayAfterDraw{false};
    int mIndexOfNewlyDrawn{-1};

    friend std::ostream& operator<<(std::ostream& os, const PlayerStat& stat);
};

}}