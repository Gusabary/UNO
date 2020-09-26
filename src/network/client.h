#pragma once

#include <memory>
#include <asio.hpp>

#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class Client {
public:
    explicit Client(std::string host, std::string port);

    void Connect();

    void DeliverJoinGameInfo(const JoinGameInfo &info);

    GameStartInfo ReceiveGameStartInfo();

private:
    // read from mSocket to mReadBuffer
    void Read();

    // write from mWriteBuffer to mSocket
    void Write();

public:
    std::function<void()> OnConnect;

private:
    constexpr static int MAX_BUFFER_SIZE = 256;

    const std::string mHost;
    const std::string mPort;

    asio::io_context mContext;
    tcp::socket mSocket;
    uint8_t mReadBuffer[MAX_BUFFER_SIZE];
    uint8_t mWriteBuffer[MAX_BUFFER_SIZE];
};
}}