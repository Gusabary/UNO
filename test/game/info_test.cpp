#include <gtest/gtest.h>
#include <memory>

#include "../../src/game/stat.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class InfoFixture : public ::testing::Test {
public:
    InfoFixture() {}

    void SetUp() {}

    void TearDown() {}

    template<typename InfoT, typename... Types>
    void Harness(Types&&... args) {
        InfoT info(args...);
        info.Serialize(mBuffer);
        auto newInfo = *InfoT::Deserialize(mBuffer);
        EXPECT_EQ(info, newInfo);
    }

    uint8_t mBuffer[100];
};

TEST_F(InfoFixture, JoinGameInfo) {
    Harness<JoinGameInfo>("tbc");
}

TEST_F(InfoFixture, GameStartInfo) {
    std::array<Card, 7> initHandCards{"R1", "R2", "R3", "R4", "+4", "W", "B5"};
    std::vector<std::string> usernames{"tbc", "tyq"};
    Harness<GameStartInfo>(initHandCards, "R0", 0, usernames);
}

TEST_F(InfoFixture, ActionInfo) {
    Harness<ActionInfo>(ActionType::DRAW);
}

TEST_F(InfoFixture, DrawInfo) {
    Harness<DrawInfo>(2);
}

TEST_F(InfoFixture, SkipInfo) {
    Harness<SkipInfo>();
}

TEST_F(InfoFixture, PlayInfo) {
    Harness<PlayInfo>("Y5", CardColor::GREEN);
}

TEST_F(InfoFixture, DrawRspInfo) {
    std::vector<Card> cards{"RS", "G+2"};
    Harness<DrawRspInfo>(2, cards);
}

TEST_F(InfoFixture, GameEndInfo) {
    Harness<GameEndInfo>(0);
}

}}