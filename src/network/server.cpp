#include <iostream>
#include <thread>
#include <chrono>

#include "server.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Session::Session(tcp::socket socket, Server &server, int index)  
    : mSocket(std::move(socket)), mServer(server), mIndex(index)
{}

void Session::Start() 
{
    mServer.Join(shared_from_this());
    JoinGameInfo info = ReceiveJoinGameInfo();
    mServer.OnReceiveJoinGameInfo(mIndex, info);
}

JoinGameInfo Session::ReceiveJoinGameInfo()
{
    Read();
    return reinterpret_cast<JoinGameMsg *>(mReadBuffer)->ToInfo();
}

void Session::DeliverGameStartInfo(const GameStartInfo &info)
{
    reinterpret_cast<GameStartMsg *>(mWriteBuffer)->FromInfo(info);
    Write();
}

void Session::Read()
{
    // read header
    asio::read(mSocket, asio::buffer(mReadBuffer, sizeof(Msg)));

    // read body
    int len = reinterpret_cast<Msg *>(mReadBuffer)->mLen;
    asio::read(mSocket, asio::buffer(mReadBuffer + sizeof(Msg), len));
}

void Session::Write() 
{
    Msg *msg = reinterpret_cast<Msg *>(mWriteBuffer);
    int len = sizeof(Msg) + msg->mLen;
    asio::async_write(mSocket, asio::buffer(msg, len), 
        [this](std::error_code, std::size_t) {}
    );
}

Server::Server(std::string port) : mPort(port)
{}

void Server::Run()
{
    tcp::endpoint endpoint(tcp::v4(), std::atoi(mPort.c_str()));

    mAcceptor = std::make_unique<tcp::acceptor>(mContext, endpoint);
    Accept();

    mContext.run();
}

void Server::Join(const std::shared_ptr<Session> &session) 
{
    mSessions.push_back(session);
}

void Server::DeliverGameStartInfo(int index, const GameStartInfo &info)
{
    mSessions[index]->DeliverGameStartInfo(info);
}

void Server::Accept() 
{
    mAcceptor->async_accept([this](std::error_code ec, tcp::socket socket) {
        if (!ec) {
            // a new player joins in
            int index = mSessions.size();
            std::cout << "a new player joins in, index : " << index << std::endl;
            std::make_shared<Session>(std::move(socket), *this, index)->Start();
        }
        Accept();
    });
}

}}