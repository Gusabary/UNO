#include <gtest/gtest.h>
#include <memory>

#include "../mock.h"
#include "../../src/game/game_board.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class GameBoardFixture : public ::testing::Test {
public:
    GameBoardFixture() {}

    void SetUp() {
        mMockServer = std::make_shared<NiceMock<MockServer>>();
        mGameBoard = std::make_unique<GameBoard>(mMockServer);
    }

    void TearDown() {}
    
    // use NickMock to ignore warnings of uninterested mock methods
    std::shared_ptr<NiceMock<MockServer>> mMockServer;
    std::unique_ptr<GameBoard> mGameBoard;
};

TEST_F(GameBoardFixture, ReceiveUsername) {
    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameRspInfo), 0, _)).Times(1);
    mGameBoard->ReceiveUsername(0, "tbc");
    auto stats1 = mGameBoard->GetPlayerStats();
    EXPECT_EQ(stats1.size(), 1);
    EXPECT_EQ(stats1[0].GetUsername(), "tbc");

    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameRspInfo), 1, _)).Times(1);
    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameInfo), 0, _)).Times(1);
    mGameBoard->ReceiveUsername(1, "cbt");
    auto stats2 = mGameBoard->GetPlayerStats();
    EXPECT_EQ(stats2.size(), 2);
    EXPECT_EQ(stats2[1].GetUsername(), "cbt");

    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameRspInfo), 2, _)).Times(1);
    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameInfo), 0, _)).Times(1);
    EXPECT_CALL(*mMockServer, DeliverInfo(&typeid(JoinGameInfo), 1, _)).Times(1);
    mGameBoard->ReceiveUsername(2, "bct");
    auto stats3 = mGameBoard->GetPlayerStats();
    EXPECT_EQ(stats3.size(), 3);
    EXPECT_EQ(stats3[2].GetUsername(), "bct");
}
}}