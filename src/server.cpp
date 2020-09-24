#include <iostream>

#include "server.h"
#include "msg.hpp"

namespace UNO { namespace Network {

using asio::ip::tcp;

Session::Session(tcp::socket socket, Server &server)  
    : mSocket(std::move(socket)), mServer(server) 
{}

void Session::Start() 
{
    mServer.Join(shared_from_this());
    Read();
}

void Session::Read() 
{
    auto self(shared_from_this());
    mSocket.async_read_some(asio::buffer(mBuffer, 100),
        [this, self](std::error_code ec, std::size_t size) {
            if (!ec || ec.message() == "End of file") {
                mServer.Deliver(mBuffer);
            }
        }
    );
}

void Session::Write(const std::string &msg)
{
    auto self(shared_from_this());
    asio::async_write(mSocket, asio::buffer(msg),
        [this, msg](std::error_code ec, std::size_t size) {
            if (!ec) {
            }
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

void Server::Deliver(const std::string &msg) 
{
    // TODO: deliver to all sessions
    std::cout << "deliver: " << msg << std::endl;
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