#include <gtest/gtest.h>
#include <memory>

#include "../../src/game/stat.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class PlayerStatFixture : public ::testing::Test {
public:
    PlayerStatFixture() {}

    void SetUp() {
        mPlayerStat = std::make_unique<PlayerStat>("tbc", mDefaultRemainingHandCardsNum);
        mPlayerStat->SetLastPlayedCard(mDefaultLastPlayedCard);
    }

    void TearDown() {}

    std::unique_ptr<PlayerStat> mPlayerStat;

    const int mDefaultRemainingHandCardsNum = 7;
    const bool mDefaultDoPlayInLastRound = false;
    const Card mDefaultLastPlayedCard = "B6";
    const bool mDefaultHasChanceToPlayAfterDraw = false;
    const int mDefaultIndexOfNewlyDrawn = -1;
};

#define EXPECT_PLAYER_STAT_EQ(remainingHandCardsNum, doPlayInLastRound, \
    lastPlayedCard, hasChanceToPlayAfterDraw, indexOfNewlyDrawn) \
    EXPECT_EQ(mPlayerStat->GetRemainingHandCardsNum(), remainingHandCardsNum); \
    EXPECT_EQ(mPlayerStat->DoPlayInLastRound(), doPlayInLastRound); \
    EXPECT_EQ(mPlayerStat->GetLastPlayedCard(), lastPlayedCard); \
    EXPECT_EQ(mPlayerStat->HasChanceToPlayAfterDraw(), hasChanceToPlayAfterDraw); \
    EXPECT_EQ(mPlayerStat->GetIndexOfNewlyDrawn(), indexOfNewlyDrawn);

TEST_F(PlayerStatFixture, Draw_SingleCard) {
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum, mDefaultDoPlayInLastRound, 
        mDefaultLastPlayedCard, mDefaultHasChanceToPlayAfterDraw, mDefaultIndexOfNewlyDrawn);
    mPlayerStat->UpdateAfterDraw(1, 0);
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum + 1, false, mDefaultLastPlayedCard, true, 0);
}

TEST_F(PlayerStatFixture, Draw_MultiCards) {
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum, mDefaultDoPlayInLastRound, 
        mDefaultLastPlayedCard, mDefaultHasChanceToPlayAfterDraw, mDefaultIndexOfNewlyDrawn);
    mPlayerStat->UpdateAfterDraw(4, 0);
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum + 4, false, mDefaultLastPlayedCard, false, 0);
}

TEST_F(PlayerStatFixture, Skip) {
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum, mDefaultDoPlayInLastRound, 
        mDefaultLastPlayedCard, mDefaultHasChanceToPlayAfterDraw, mDefaultIndexOfNewlyDrawn);
    mPlayerStat->UpdateAfterSkip();
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum, false, 
        mDefaultLastPlayedCard, false, mDefaultIndexOfNewlyDrawn);
}

TEST_F(PlayerStatFixture, Play) {
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum, mDefaultDoPlayInLastRound, 
        mDefaultLastPlayedCard, mDefaultHasChanceToPlayAfterDraw, mDefaultIndexOfNewlyDrawn);
    Card cardToPlay = "R5";
    mPlayerStat->UpdateAfterPlay(cardToPlay);
    EXPECT_PLAYER_STAT_EQ(mDefaultRemainingHandCardsNum - 1, true, 
        cardToPlay, false, mDefaultIndexOfNewlyDrawn);
}

}}