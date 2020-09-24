#pragma once

#include <memory>
#include <asio.hpp>

#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, Server &server);

    void Start();

    void ReadHeader();
    
    void ReadBody();

    void Write(Msg *msg);

private:
    constexpr static int MAX_BUFFER_SIZE = 256;

    tcp::socket mSocket;
    Server &mServer;
    uint8_t mBuffer[MAX_BUFFER_SIZE];
};

class Server {
public:
    explicit Server(std::string port);

    void Join(const std::shared_ptr<Session> &session);
    
    void Deliver(uint8_t *buffer);

private:
    void Accept();

private:
    asio::io_context mContext;
    std::unique_ptr<tcp::acceptor> mAcceptor;
    std::vector<std::shared_ptr<Session>> mSessions;
};
}}