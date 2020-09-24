#include <iostream>

#include "client.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Client::Client(std::string host, std::string port) 
    : mSocket(mContext)
{
    tcp::resolver resolver(mContext);
    auto endpoints = resolver.resolve(host, port);

    memset(mBuffer, 0, 100);

    asio::async_connect(mSocket, endpoints,
        [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                Write("12345678");
            }
        }
    );

    std::thread t([this]() { mContext.run(); });
    t.join();
}

void Client::Read() 
{
    mSocket.async_read_some(asio::buffer(mBuffer, 100),
        [this](std::error_code ec, std::size_t) {
            if (!ec || ec.message() == "End of file") {
            }
        }
    );
}

void Client::Write(const std::string &msg) 
{
    asio::async_write(mSocket, asio::buffer(msg), 
        [this](std::error_code, std::size_t) {
            Read();
        }
    );
}
}}