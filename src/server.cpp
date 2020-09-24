#include <iostream>

#include "server.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Session::Session(tcp::socket socket, Server &server)  
    : mSocket(std::move(socket)), mServer(server) 
{}

void Session::Start() 
{
    mServer.Join(shared_from_this());
    ReadHeader();
}

void Session::ReadHeader()
{
    asio::async_read(mSocket, asio::buffer(mBuffer, sizeof(Msg)),
        [this](std::error_code ec, std::size_t) {
            if (!ec) {
                ReadBody();
            }
        }
    );
}

void Session::ReadBody()
{
    int len = reinterpret_cast<Msg *>(mBuffer)->mLen;
    asio::async_read(mSocket, asio::buffer(mBuffer + sizeof(Msg), len),
        [this](std::error_code ec, std::size_t) {
            if (!ec) {
                // TODO: handle different conditions
                JoinGameMsg *msg = reinterpret_cast<JoinGameMsg *>(mBuffer);
                mServer.Deliver(mBuffer);
                // ReadHeader();
            }
        }
    );
}

void Session::Write(Msg *msg) 
{
    int len = sizeof(Msg) + msg->mLen;
    asio::async_write(mSocket, asio::buffer(msg, len), 
        [this](std::error_code, std::size_t) {
            ReadHeader();
        }
    );
}

Server::Server(std::string port) 
{
    tcp::endpoint endpoint(tcp::v4(), std::atoi(port.c_str()));

    mAcceptor = std::make_unique<tcp::acceptor>(mContext, endpoint);
    Accept();

    mContext.run();
}

void Server::Join(const std::shared_ptr<Session> &session) 
{
    mSessions.push_back(session);
}

void Server::Deliver(uint8_t *buffer) 
{
    // TODO: deliver to all sessions
    Msg *msg = reinterpret_cast<Msg *>(buffer);
    std::for_each(mSessions.begin(), mSessions.end(), 
        [msg](const std::shared_ptr<Session> &session) {
            session->Write(msg);
        }
    );
}

void Server::Accept() 
{
    mAcceptor->async_accept([this](std::error_code ec, tcp::socket socket) {
        if (!ec) {
            // a new player joins in
            std::cout << "a new player joins in" << std::endl;
            std::make_shared<Session>(std::move(socket), *this)->Start();
        }
        Accept();
    });
}

}}