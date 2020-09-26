#pragma once

#include <memory>
#include <asio.hpp>

#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, Server &server, int index);

    void Start();

    JoinGameInfo ReceiveJoinGameInfo();

    void DeliverGameStartInfo(const GameStartInfo &info);

private:
    // read from mSocket to mReadBuffer
    void Read();

    // write from mWriteBuffer to mSocket
    void Write();

private:
    constexpr static int MAX_BUFFER_SIZE = 256;

    tcp::socket mSocket;
    Server &mServer;
    const int mIndex;
    uint8_t mReadBuffer[MAX_BUFFER_SIZE];
    uint8_t mWriteBuffer[MAX_BUFFER_SIZE];
};

class Server {
public:
    explicit Server(std::string port);

    void Join(const std::shared_ptr<Session> &session);
    
    void DeliverGameStartInfo(int index, const GameStartInfo &info);

    void Run();

private:
    void Accept();

public:
    // callbacks in server side should always take index of session as the first parameter
    std::function<void(int, const JoinGameInfo &)> OnReceiveJoinGameInfo;

private:
    const std::string mPort;
    
    asio::io_context mContext;
    std::unique_ptr<tcp::acceptor> mAcceptor;
    std::vector<std::shared_ptr<Session>> mSessions;
};
}}