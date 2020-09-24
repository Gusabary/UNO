#pragma once

#include <memory>
#include <asio.hpp>

namespace UNO { namespace Network {

using asio::ip::tcp;

class Client {
public:
    explicit Client(std::string host, std::string port);

    void Read();

    void Write(const std::string &msg);

private:

private:
    asio::io_context mContext;
    tcp::socket mSocket;
    char mBuffer[100];
};
}}