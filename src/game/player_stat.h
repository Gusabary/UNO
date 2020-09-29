#pragma once

#include "../network/msg.h"

namespace UNO { namespace Game {

using namespace Network;

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