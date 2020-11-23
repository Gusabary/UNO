#include <iostream>
#include <map>

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

std::unique_ptr<Info> Client::ReceiveInfo(const std::type_info &infoType)
{
    using funcType = std::function<std::unique_ptr<Info>()>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),  [this] { return ReceiveInfoImpl<JoinGameInfo>(); }},
        {&typeid(GameStartInfo), [this] { return ReceiveInfoImpl<GameStartInfo>(); }},
        {&typeid(ActionInfo),    [this] { return ReceiveInfoImpl<ActionInfo>(); }},
        {&typeid(DrawInfo),      [this] { return ReceiveInfoImpl<DrawInfo>(); }},
        {&typeid(SkipInfo),      [this] { return ReceiveInfoImpl<SkipInfo>(); }},
        {&typeid(PlayInfo),      [this] { return ReceiveInfoImpl<PlayInfo>(); }},
        {&typeid(DrawRspInfo),   [this] { return ReceiveInfoImpl<DrawRspInfo>(); }}
    };

    auto it = mapping.find(&infoType);
    assert(it != mapping.end());
    return it->second();
}

void Client::DeliverInfo(const std::type_info &infoType, const Info &info)
{
    using funcType = std::function<void(const Info &)>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),  [this](const Info &info) { 
            DeliverInfoImpl<JoinGameInfo>(dynamic_cast<const JoinGameInfo &>(info)); 
        }},
        {&typeid(GameStartInfo), [this](const Info &info) { 
            DeliverInfoImpl<GameStartInfo>(dynamic_cast<const GameStartInfo &>(info)); 
        }},
        {&typeid(ActionInfo),    [this](const Info &info) { 
            DeliverInfoImpl<ActionInfo>(dynamic_cast<const ActionInfo &>(info)); 
        }},
        {&typeid(DrawInfo),      [this](const Info &info) { 
            DeliverInfoImpl<DrawInfo>(dynamic_cast<const DrawInfo &>(info)); 
        }},
        {&typeid(SkipInfo),      [this](const Info &info) { 
            DeliverInfoImpl<SkipInfo>(dynamic_cast<const SkipInfo &>(info)); 
        }},
        {&typeid(PlayInfo),      [this](const Info &info) { 
            DeliverInfoImpl<PlayInfo>(dynamic_cast<const PlayInfo &>(info)); 
        }},
        {&typeid(DrawRspInfo),   [this](const Info &info) { 
            DeliverInfoImpl<DrawRspInfo>(dynamic_cast<const DrawRspInfo &>(info)); 
        }}
    };
    auto it = mapping.find(&infoType);
    assert(it != mapping.end());
    return it->second(info);
}
}}