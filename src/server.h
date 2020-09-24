#pragma once

#include <memory>
#include <asio.hpp>

namespace UNO { namespace Network {

using asio::ip::tcp;

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, Server &server);

    void Start();

    void Read();

    void Write(const std::string &msg);

private:
    tcp::socket mSocket;
    Server &mServer;
    char mBuffer[100];
};

class Server {
public:
    explicit Server(std::string port);

    void Join(const std::shared_ptr<Session> &session);
    
    void Deliver(const std::string &msg);

private:
    void Accept();

private:
    asio::io_context mContext;
    std::unique_ptr<tcp::acceptor> mAcceptor;
    std::vector<std::shared_ptr<Session>> mSessions;
};
}}