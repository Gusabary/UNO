#include <gtest/gtest.h>
#include <memory>

#include "../../src/game/stat.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class GameStatFixture : public ::testing::Test {
public:
    GameStatFixture() {}

    void SetUp() {
        mGameStat = std::make_unique<GameStat>(mDefaultFirstPlayer, mDefaultFlippedCard);
        mGameStat->SetLastPlayedCard(mDefaultLastPlayedCard);
    }

    void TearDown() {}

    std::unique_ptr<GameStat> mGameStat;

    const int mDefaultFirstPlayer = 1;
    const Card mDefaultFlippedCard = "R3";
    const bool mDefaultIsInClockwise = true;
    const Card mDefaultLastPlayedCard = "B6";
};

#define EXPECT_GAME_STAT_EQ(currentPlayer, isInClockwise, lastPlayedCard, cardsNumToDraw) \
    EXPECT_EQ(mGameStat->GetCurrentPlayer(), (currentPlayer)); \
    EXPECT_EQ(mGameStat->IsInClockwise(), (isInClockwise)); \
    EXPECT_EQ(mGameStat->GetLastPlayedCard(), (lastPlayedCard)); \
    EXPECT_EQ(mGameStat->GetCardsNumToDraw(), (cardsNumToDraw));

TEST_F(GameStatFixture, Draw_Common) {
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterDraw();
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
}

TEST_F(GameStatFixture, Draw_ConsumePenalty_Plus2) {
    Card lastPlayedCard = "Y+2";
    mGameStat->SetLastPlayedCard(lastPlayedCard);
    mGameStat->SetCardsNumToDraw(2);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, lastPlayedCard, 2);
    mGameStat->UpdateAfterDraw();
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, "Y", 1);
}

TEST_F(GameStatFixture, Draw_ConsumePenalty_Plus4) {
    Card lastPlayedCard = "G+4";
    mGameStat->SetLastPlayedCard(lastPlayedCard);
    mGameStat->SetCardsNumToDraw(4);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, lastPlayedCard, 4);
    mGameStat->UpdateAfterDraw();
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, "G", 1);
}

TEST_F(GameStatFixture, Skip_Common) {
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterSkip();
    // currentPlayer may be wrapped here
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
}

TEST_F(GameStatFixture, Skip_ConsumePenalty) {
    Card lastPlayedCard = "RS";
    mGameStat->SetLastPlayedCard(lastPlayedCard);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, lastPlayedCard, 1);
    mGameStat->UpdateAfterSkip();
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, "R", 1);
}

TEST_F(GameStatFixture, Play_Common) {
    Card cardToPlay = "G2";
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 1);
}

TEST_F(GameStatFixture, Play_Wild) {
    Card cardToPlay = "BW";
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterPlay(cardToPlay);
    Card lastPlayedCard{CardColor::BLUE, mDefaultLastPlayedCard.mText};
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, lastPlayedCard, 1);
}

TEST_F(GameStatFixture, Play_Reverse) {
    Card cardToPlay = "YR";
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer - 1, !mDefaultIsInClockwise, cardToPlay, 1);
}

TEST_F(GameStatFixture, Play_Plus2) {
    Card cardToPlay = "R+2";
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 2);
    
    mGameStat->SetCurrentPlayer(mDefaultFirstPlayer);
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 4);
}

TEST_F(GameStatFixture, Play_Plus4) {
    Card cardToPlay = "B+4";
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer, mDefaultIsInClockwise, mDefaultLastPlayedCard, 1);
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 4);

    mGameStat->SetCurrentPlayer(mDefaultFirstPlayer);
    cardToPlay = "Y+4";
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 8);

    mGameStat->SetCurrentPlayer(mDefaultFirstPlayer);
    cardToPlay = "Y+2";
    mGameStat->UpdateAfterPlay(cardToPlay);
    EXPECT_GAME_STAT_EQ(mDefaultFirstPlayer + 1, mDefaultIsInClockwise, cardToPlay, 10);
}

}}