#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"
#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

class Server {
public:
    explicit Server(std::string port);

    void Run();

    template <typename MsgT>
    typename MsgT::InfoT ReceiveInfo(int index) {
        return mSessions[index]->ReceiveInfo<MsgT>();
    }

    template <typename MsgT>
    void DeliverInfo(int index, const typename MsgT::InfoT &info) {
        mSessions[index]->DeliverInfo<MsgT>(info);
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