#include "stat.h"

namespace UNO { namespace Game {

GameStat::GameStat(const GameStartInfo &info)
    : mCurrentPlayer(info.mFirstPlayer), 
    mIsInClockwise(info.mFlippedCard.mText != CardText::REVERSE),
    mLastPlayedCard(info.mFlippedCard) {}

GameStat::GameStat(int firstPlayer, Card flippedCard)
    : mCurrentPlayer(firstPlayer),
    mIsInClockwise(flippedCard.mText != CardText::REVERSE) {}

void GameStat::NextPlayer()
{
    mCurrentPlayer = mIsInClockwise ? 
        Common::Util::WrapWithPlayerNum(mCurrentPlayer + 1) :
        Common::Util::WrapWithPlayerNum(mCurrentPlayer - 1);
    mTimeElapsed = 0;
}

void GameStat::UpdateAfterDraw()
{
    if (CardSet::DrawTexts.count(mLastPlayedCard.mText)) {
        // last played card will become EMPTY after the draw penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    // the number of cards to draw falls back to 1
    mCardsNumToDraw = 1;

    // no need to invoke NextPlayer() here 
    // because a draw action is always followed by a skip or play action
}

void GameStat::UpdateAfterSkip()
{
    if (mLastPlayedCard.mText == CardText::SKIP) {
        // last played card will become EMPTY after the skip penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    NextPlayer();
}

void GameStat::UpdateAfterPlay(Card card)
{
    if (card.mText == CardText::WILD) {
        // if just a common wild card (not +4), don't affect the number text
        mLastPlayedCard.mColor = card.mColor;
    }
    else {
        mLastPlayedCard = card;
    }
    
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

    if (!mGameEnds) {
        NextPlayer();
    }
}

void GameStat::Tick()
{
    mTimeElapsed++;
}

PlayerStat::PlayerStat(const std::string &username, int remainingHandCardsNum)
    : mUsername(username), mRemainingHandCardsNum(remainingHandCardsNum) {}

void PlayerStat::UpdateAfterDraw(int number, int indexOfNewlyDrawn)
{
    mRemainingHandCardsNum += number;
    mDoPlayInLastRound = false;
    // only common draw (rather than draw penalty due to +2 / +4) 
    // has the chance to play the card just drawn immediately
    mHasChanceToPlayAfterDraw = (number == 1);
    mIndexOfNewlyDrawn = indexOfNewlyDrawn;
}

void PlayerStat::UpdateAfterSkip()
{
    mDoPlayInLastRound = false;
    mHasChanceToPlayAfterDraw = false;
}

void PlayerStat::UpdateAfterPlay(Card card)
{
    mRemainingHandCardsNum--;
    mDoPlayInLastRound = true;
    mLastPlayedCard = card;
    mHasChanceToPlayAfterDraw = false;
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