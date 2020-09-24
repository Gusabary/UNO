#pragma once

#include <memory>
#include <asio.hpp>

#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

class Client {
public:
    explicit Client(std::string host, std::string port);

    void ReadHeader();

    void ReadBody();

    void Write();

private:

private:
    constexpr static int MAX_BUFFER_SIZE = 256;

    asio::io_context mContext;
    tcp::socket mSocket;
    uint8_t mBuffer[MAX_BUFFER_SIZE];
};
}}