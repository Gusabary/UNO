#include "stat.h"

namespace UNO { namespace Game {

GameStat::GameStat(const GameStartInfo &info)
    : mCurrentPlayer(info.mFirstPlayer), 
    mIsInClockwise(info.mFlippedCard.mText != CardText::REVERSE),
    mLastPlayedCard(info.mFlippedCard) {}

GameStat::GameStat(int firstPlayer, Card flippedCard)
    : mCurrentPlayer(firstPlayer),
    mIsInClockwise(flippedCard.mText != CardText::REVERSE) {}

void GameStat::NextPlayer(int playerNum)
{
    mCurrentPlayer = mIsInClockwise ? 
        Util::WrapWithPlayerNum(mCurrentPlayer + 1, playerNum) :
        Util::WrapWithPlayerNum(mCurrentPlayer - 1, playerNum);
}

void GameStat::UpdateAfterPlay(Card card)
{
    mLastPlayedCard = card;
    if (card.mText == CardText::REVERSE) {
        mIsInClockwise = !mIsInClockwise;
    }
    if (card.mText == CardText::DRAW_TWO) {
        // in the normal state, mCardsNumToDraw is equal to 1
        // once a player plays a `Draw` card, the effect is gonna accumulate
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 2 : (mCardsNumToDraw + 2);
    }
    if (card.mText == CardText::DRAW_FOUR) {
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 4 : (mCardsNumToDraw + 4);
    }
}


PlayerStat::PlayerStat(const std::string &username, int remainingHandCardsNum)
    : mUsername(username), mRemainingHandCardsNum(remainingHandCardsNum) {}

void PlayerStat::UpdateAfterDraw(int number)
{
    mRemainingHandCardsNum += number;
    mDoPlayInLastRound = false;
}

void PlayerStat::UpdateAfterSkip()
{
    mDoPlayInLastRound = false;
}

void PlayerStat::UpdateAfterPlay(Card card)
{
    mRemainingHandCardsNum--;
    mDoPlayInLastRound = true;
    mLastPlayedCard = card;
}

std::ostream& operator<<(std::ostream& os, const PlayerStat& stat)
{
    os << "\t   { " << stat.mUsername << ", " << stat.mRemainingHandCardsNum;
    if (stat.mDoPlayInLastRound) {
        os << ", " << stat.mLastPlayedCard;
    }
    os << " }";
    return os;
}

}}