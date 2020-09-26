#include <iostream>

#include "client.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Client::Client(std::string host, std::string port) 
    : mHost(host), mPort(port)
{}

void Client::Connect()
{
    tcp::resolver resolver(mContext);
    auto endpoints = resolver.resolve(mHost, mPort);
    tcp::socket socket(mContext);

    asio::async_connect(socket, endpoints,
        [this, &socket](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                mSession = std::make_unique<Session>(std::move(socket));
                OnConnect();
            }
        }
    );

    mContext.run();
}

}}