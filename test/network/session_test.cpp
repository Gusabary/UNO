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
        // is it necessary to **sleep** to ensure that connect is after accept?
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
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

TEST_F(SessionFixture, JoinGame) {
    mClientSession->DeliverInfo<JoinGameInfo>(JoinGameInfo("tbc"));
    // here the invokation of run ensures that write will finish before receive
    mClientContext.run();
    
    // make sure that mServerSession has been reset before invoking ReceiveInfo
    mServerThread->join();
    std::unique_ptr<JoinGameInfo> info = mServerSession->ReceiveInfo<JoinGameInfo>();
    // only async operation needs invokation of run, while read here is blocking
    // mServerContext.run();

    EXPECT_EQ(info->mUsername, "tbc");
}

TEST_F(SessionFixture, GameStart) {
    std::array<Card, 7> initHandCards{"R1", "W", "+4", "R+2", "YS", "GR", "BW"};
    std::vector<std::string> usernames{"fred", "daniel", "greg", "cissie"};
    mClientSession->DeliverInfo<GameStartInfo>(GameStartInfo(initHandCards, "B0", 1, usernames));
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<GameStartInfo> info = mServerSession->ReceiveInfo<GameStartInfo>();

    EXPECT_EQ(info->mInitHandCards, initHandCards);
    EXPECT_EQ(info->mFlippedCard, "B0");
    EXPECT_EQ(info->mFirstPlayer, 1);
    EXPECT_EQ(info->mUsernames, usernames);
}

TEST_F(SessionFixture, Action) {
    mClientSession->DeliverInfo<ActionInfo>(ActionInfo(ActionType::DRAW));
    mClientContext.run();

    mServerThread->join();
    std::unique_ptr<ActionInfo> info = mServerSession->ReceiveInfo<ActionInfo>();

    EXPECT_EQ(info->mActionType, ActionType::DRAW);
    EXPECT_EQ(info->mPlayerIndex, -1);
}

}}