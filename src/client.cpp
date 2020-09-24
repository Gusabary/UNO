#include <iostream>

#include "client.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Client::Client(std::string host, std::string port) 
    : mSocket(mContext)
{
    tcp::resolver resolver(mContext);
    auto endpoints = resolver.resolve(host, port);

    asio::async_connect(mSocket, endpoints,
        [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                std::string username = "tbc";
                reinterpret_cast<JoinGameMsg *>(mBuffer)->Gear(username);
                Write();
            }
        }
    );

    std::thread t([this]() { mContext.run(); });
    t.join();
}

void Client::ReadHeader()
{
    asio::async_read(mSocket, asio::buffer(mBuffer, sizeof(Msg)),
        [this](std::error_code ec, std::size_t) {
            if (!ec) {
                ReadBody();
            }
        }
    );
}

void Client::ReadBody()
{
    int len = reinterpret_cast<Msg *>(mBuffer)->mLen;
    asio::async_read(mSocket, asio::buffer(mBuffer + sizeof(Msg), len),
        [this](std::error_code ec, std::size_t) {
            if (!ec) {
                // TODO: handle different conditions
                JoinGameMsg *msg = reinterpret_cast<JoinGameMsg *>(mBuffer);
                ReadHeader();
            }
        }
    );
}

void Client::Write() 
{
    Msg *msg = reinterpret_cast<Msg *>(mBuffer);
    int len = sizeof(Msg) + msg->mLen;
    asio::async_write(mSocket, asio::buffer(msg, len), 
        [this](std::error_code, std::size_t) {
            ReadHeader();
        }
    );
}
}}