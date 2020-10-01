#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "asio.hpp"
#include "../../src/network/session.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Network;
using namespace Game;
using asio::ip::tcp;

class SessionFixture : public ::testing::Test {
public:
    SessionFixture() {}

    void SetUp() {
        mServerThread.reset(new std::thread([this]() { SetUpServer(); }));
        // it's necessary to **sleep** to ensure that connect is after accept
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        SetUpClient();
    }

    void TearDown() {}

    void SetUpServer() {
        tcp::acceptor acceptor(mServerContext, tcp::endpoint(tcp::v4(), PORT_NUM));
        tcp::socket socket(mServerContext);
        acceptor.accept(socket);

        mServerSession.reset(new Session(std::move(socket)));
    }

    void SetUpClient() {
        tcp::resolver resolver(mClientContext);
        auto endpoints = resolver.resolve("localhost", std::to_string(PORT_NUM));
        tcp::socket socket(mClientContext);
        asio::connect(socket, endpoints);

        mClientSession.reset(new Session(std::move(socket)));
    }

    constexpr static short PORT_NUM = 6012;
    asio::io_context mServerContext;
    asio::io_context mClientContext;
    std::unique_ptr<Session> mServerSession;
    std::unique_ptr<Session> mClientSession;
    std::unique_ptr<std::thread> mServerThread;
};

TEST_F(SessionFixture, JoinGameInfo) {
    mClientSession->DeliverInfo<JoinGameInfo>("tbc");
    // here the invokation of run ensures that write will finish before receive
    mClientContext.run();
    
    // make sure that mServerSession has been reset before invoking ReceiveInfo
    mServerThread->join();
    std::unique_ptr<JoinGameInfo> info = mServerSession->ReceiveInfo<JoinGameInfo>();
    // only async operation needs invokation of run, while read here is blocking
    // mServerContext.run();

    EXPECT_EQ(info->mUsername, "tbc");
}

TEST_F(SessionFixture, GameStartInfo) {
    std::array<Card, 7> initHandCards{"R1", "W", "+4", "R+2", "YS", "GR", "BW"};
    std::vector<std::string> usernames{"fred", "daniel", "greg", "cissie"};
    mClientSession->DeliverInfo<GameStartInfo>(initHandCards, "B0", 1, usernames);
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<GameStartInfo> info = mServerSession->ReceiveInfo<GameStartInfo>();

    EXPECT_EQ(info->mInitHandCards, initHandCards);
    EXPECT_EQ(info->mFlippedCard, "B0");
    EXPECT_EQ(info->mFirstPlayer, 1);
    EXPECT_EQ(info->mUsernames, usernames);
}

TEST_F(SessionFixture, ActionInfo) {
    mClientSession->DeliverInfo<ActionInfo>(ActionType::DRAW);
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<ActionInfo> info = mServerSession->ReceiveInfo<ActionInfo>();

    EXPECT_EQ(info->mActionType, ActionType::DRAW);
    EXPECT_EQ(info->mPlayerIndex, -1);
}

TEST_F(SessionFixture, DrawInfo) {
    mClientSession->DeliverInfo<DrawInfo>(2);
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<DrawInfo> info = mServerSession->ReceiveInfo<DrawInfo>();

    EXPECT_EQ(info->mActionType, ActionType::DRAW);
    EXPECT_EQ(info->mPlayerIndex, -1);
    EXPECT_EQ(info->mNumber, 2);
}

TEST_F(SessionFixture, SkipInfo) {
    mClientSession->DeliverInfo<SkipInfo>();
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<SkipInfo> info = mServerSession->ReceiveInfo<SkipInfo>();
    
    EXPECT_EQ(info->mActionType, ActionType::SKIP);
    EXPECT_EQ(info->mPlayerIndex, -1);
}

TEST_F(SessionFixture, PlayInfoWithoutNextColor) {
    mClientSession->DeliverInfo<PlayInfo>("R6");
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<PlayInfo> info = mServerSession->ReceiveInfo<PlayInfo>();

    EXPECT_EQ(info->mActionType, ActionType::PLAY);
    EXPECT_EQ(info->mPlayerIndex, -1);
    EXPECT_EQ(info->mCard, "R6");
    EXPECT_EQ(info->mNextColor, CardColor::RED);
}

TEST_F(SessionFixture, PlayInfoWithNextColor) {
    mClientSession->DeliverInfo<PlayInfo>("W", CardColor::BLUE);
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<PlayInfo> info = mServerSession->ReceiveInfo<PlayInfo>();

    EXPECT_EQ(info->mActionType, ActionType::PLAY);
    EXPECT_EQ(info->mPlayerIndex, -1);
    EXPECT_EQ(info->mCard, "W");
    EXPECT_EQ(info->mNextColor, CardColor::BLUE);
}

TEST_F(SessionFixture, DrawRspInfo) {
    mServerThread->join();
    std::vector<Card> cards{"R2", "+4"};
    mServerSession->DeliverInfo<DrawRspInfo>(2, cards);
    mServerContext.run();

    std::unique_ptr<DrawRspInfo> info = mClientSession->ReceiveInfo<DrawRspInfo>();

    EXPECT_EQ(info->mNumber, 2);
    EXPECT_EQ(info->mCards, cards);
}
}}