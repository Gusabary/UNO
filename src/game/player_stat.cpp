#include "player_stat.h"

namespace UNO { namespace Game {

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
    os << "\t { " << stat.mUsername << ", " << stat.mRemainingHandCardsNum;
    if (stat.mDoPlayInLastRound) {
        os << ", " << stat.mLastPlayedCard;
    }
    os << " }";
    return os;
}

}}