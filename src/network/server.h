#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class Server {
public:
    explicit Server(std::string port);

    void Run();

    template <typename InfoT>
    std::unique_ptr<InfoT> ReceiveInfo(int index) {
        return mSessions[index]->ReceiveInfo<InfoT>();
    }

    template <typename InfoT>
    void DeliverInfo(int index, const InfoT &info) {
        mSessions[index]->DeliverInfo<InfoT>(info);
    }

private:
    void Accept();

public:
    // callbacks in server side should always take index of session as the first parameter
    std::function<void(int, const JoinGameInfo &)> OnReceiveJoinGameInfo;

private:
    const std::string mPort;
    
    asio::io_context mContext;
    std::unique_ptr<tcp::acceptor> mAcceptor;
    std::vector<std::unique_ptr<Session>> mSessions;
};
}}