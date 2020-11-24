#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/network/server.h"
#include "../src/network/client.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class MockServer : public IServer {
public:
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(void, RegisterReceiveJoinGameInfoCallback, 
        (const std::function<void(int, const JoinGameInfo &)> &), (override));
    MOCK_METHOD(void, RegisterAllPlayersJoinedCallback, (const std::function<void()> &), (override));
    MOCK_METHOD(std::unique_ptr<Info>, ReceiveInfo, (const std::type_info *, int), (override));
    MOCK_METHOD(void, DeliverInfo, (const std::type_info *, int, const Info &), (override));
};

class MockClient : public IClient {
public:
    MOCK_METHOD(void, Connect, (), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(void, RegisterConnectCallback, (const std::function<void()> &), (override));
    MOCK_METHOD(std::unique_ptr<Info>, ReceiveInfo, (const std::type_info *), (override));
    MOCK_METHOD(void, DeliverInfo, (const std::type_info *, const Info &), (override));
};

}}