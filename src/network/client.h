#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"
#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class Client {
public:
    explicit Client(std::string host, std::string port);

    void Connect();

    template <typename MsgT>
    typename MsgT::InfoT ReceiveInfo() {
        return mSession->ReceiveInfo<MsgT>();
    }

    template <typename MsgT>
    void DeliverInfo(const typename MsgT::InfoT &info) {
        mSession->DeliverInfo<MsgT>(info);
    }

public:
    std::function<void()> OnConnect;

private:
    const std::string mHost;
    const std::string mPort;

    asio::io_context mContext;
    std::unique_ptr<Session> mSession;
};
}}