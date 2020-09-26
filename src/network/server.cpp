#include <iostream>
#include <thread>
#include <chrono>

#include "server.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Server::Server(std::string port) : mPort(port)
{}

void Server::Run()
{
    tcp::endpoint endpoint(tcp::v4(), std::atoi(mPort.c_str()));

    mAcceptor = std::make_unique<tcp::acceptor>(mContext, endpoint);
    Accept();

    mContext.run();
}

void Server::Accept() 
{
    mAcceptor->async_accept([this](std::error_code ec, tcp::socket socket) {
        if (!ec) {
            // a new player joins in
            int index = mSessions.size();
            std::cout << "a new player joins in, index : " << index << std::endl;

            mSessions.push_back(std::make_unique<Session>(std::move(socket)));
            JoinGameInfo info = mSessions.back()->ReceiveInfo<JoinGameMsg>();
            OnReceiveJoinGameInfo(index, info);
        }
        Accept();
    });
}

}}