#include <iostream>

#include "client.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Client::Client(std::string host, std::string port) 
    : mHost(host), mPort(port), mSocket(mContext)
{}

void Client::Connect()
{
    tcp::resolver resolver(mContext);
    auto endpoints = resolver.resolve(mHost, mPort);

    asio::async_connect(mSocket, endpoints,
        [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                OnConnect();
            }
        }
    );

    std::thread t([this]() { mContext.run(); });
    t.join();
}

void Client::DeliverJoinGameInfo(const JoinGameInfo &info)
{
    reinterpret_cast<JoinGameMsg *>(mWriteBuffer)->FromInfo(info);
    Write();
}

GameStartInfo Client::ReceiveGameStartInfo()
{
    Read();
    return reinterpret_cast<GameStartMsg *>(mReadBuffer)->ToInfo();
}

void Client::Read()
{
    // read header
    asio::read(mSocket, asio::buffer(mReadBuffer, sizeof(Msg)));

    // read body
    int len = reinterpret_cast<Msg *>(mReadBuffer)->mLen;
    asio::read(mSocket, asio::buffer(mReadBuffer + sizeof(Msg), len));
}

void Client::Write() 
{
    Msg *msg = reinterpret_cast<Msg *>(mWriteBuffer);
    int len = sizeof(Msg) + msg->mLen;
    asio::async_write(mSocket, asio::buffer(msg, len), 
        [this](std::error_code, std::size_t) {}
    );
}
}}