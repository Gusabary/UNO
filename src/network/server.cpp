#include <iostream>
#include <map>

#include "server.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Server::Server(std::string port) : mPort(port)
{}

void Server::Run()
{
    tcp::endpoint endpoint(tcp::v4(), std::atoi(mPort.c_str()));

    mAcceptor = std::make_unique<tcp::acceptor>(mContext, endpoint);
    while (mShouldReset) {
        mShouldReset = false;
        Accept();
        mContext.run();

        std::cout << "All players have joined. Game Start!" << std::endl;
        OnAllPlayersJoined();
        Close();
    }
}

void Server::Accept() 
{
    mAcceptor->async_accept([this](std::error_code ec, tcp::socket socket) {
        if (!ec) {
            // a new player joins in
            int index = mSessions.size();
            std::cout << "a new player joins in, index : " << index << std::endl;

            mSessions.push_back(std::make_unique<Session>(std::move(socket)));
            std::unique_ptr<JoinGameInfo> info = mSessions.back()->ReceiveInfo<JoinGameInfo>();
            OnReceiveJoinGameInfo(index, *info);
        }
        if (mSessions.size() < Common::Common::mPlayerNum) {
            Accept();
        }
    });
}

void Server::Close()
{
    mAcceptor->cancel();
    mSessions.clear();
}

void Server::Reset()
{
    mShouldReset = true;
    // a invokation to restart is needed for subsequent run
    mContext.restart();
}

std::unique_ptr<Info> Server::ReceiveInfo(const std::type_info *infoType, int index)
{
    using funcType = std::function<std::unique_ptr<Info>(int)>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),    [this](int index) { return ReceiveInfoImpl<JoinGameInfo>(index); }},
        {&typeid(JoinGameRspInfo), [this](int index) { return ReceiveInfoImpl<JoinGameRspInfo>(index); }},
        {&typeid(GameStartInfo),   [this](int index) { return ReceiveInfoImpl<GameStartInfo>(index); }},
        {&typeid(ActionInfo),      [this](int index) { return ReceiveInfoImpl<ActionInfo>(index); }},
        {&typeid(DrawInfo),        [this](int index) { return ReceiveInfoImpl<DrawInfo>(index); }},
        {&typeid(SkipInfo),        [this](int index) { return ReceiveInfoImpl<SkipInfo>(index); }},
        {&typeid(PlayInfo),        [this](int index) { return ReceiveInfoImpl<PlayInfo>(index); }},
        {&typeid(DrawRspInfo),     [this](int index) { return ReceiveInfoImpl<DrawRspInfo>(index); }}
        // {&typeid(GameEndInfo),   [this, index] { return ReceiveInfoImpl<GameEndInfo>(index); }}
    };
    auto it = mapping.find(infoType);
    assert(it != mapping.end());
    // std::cout << "[RECEIVE INFO from " << index << "] " << infoType->name() << std::endl;
    return it->second(index);
}

void Server::DeliverInfo(const std::type_info *infoType, int index, const Info &info)
{
    using funcType = std::function<void(int, const Info &)>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),    [this](int index, const Info &info) { 
            DeliverInfoImpl<JoinGameInfo>(index, dynamic_cast<const JoinGameInfo &>(info)); 
        }},
        {&typeid(JoinGameRspInfo), [this](int index, const Info &info) { 
            DeliverInfoImpl<JoinGameRspInfo>(index, dynamic_cast<const JoinGameRspInfo &>(info)); 
        }},
        {&typeid(GameStartInfo),   [this](int index, const Info &info) { 
            DeliverInfoImpl<GameStartInfo>(index, dynamic_cast<const GameStartInfo &>(info)); 
        }},
        {&typeid(ActionInfo),      [this](int index, const Info &info) { 
            DeliverInfoImpl<ActionInfo>(index, dynamic_cast<const ActionInfo &>(info)); 
        }},
        {&typeid(DrawInfo),        [this](int index, const Info &info) { 
            DeliverInfoImpl<DrawInfo>(index, dynamic_cast<const DrawInfo &>(info)); 
        }},
        {&typeid(SkipInfo),        [this](int index, const Info &info) { 
            DeliverInfoImpl<SkipInfo>(index, dynamic_cast<const SkipInfo &>(info)); 
        }},
        {&typeid(PlayInfo),        [this](int index, const Info &info) { 
            DeliverInfoImpl<PlayInfo>(index, dynamic_cast<const PlayInfo &>(info)); 
        }},
        {&typeid(DrawRspInfo),     [this](int index, const Info &info) { 
            DeliverInfoImpl<DrawRspInfo>(index, dynamic_cast<const DrawRspInfo &>(info)); 
        }}
        // {&typeid(GameEndInfo),   [this, index, &info]() { 
        //     DeliverInfoImpl<GameEndInfo>(index, dynamic_cast<const GameEndInfo &>(info));
        // }}
    };
    auto it = mapping.find(infoType);
    assert(it != mapping.end());
    // std::cout << "[DELIVER INFO to   " << index << "] " << infoType->name() << std::endl;
    return it->second(index, info);
}

}}