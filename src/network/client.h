#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class Client {
public:
    explicit Client(std::string host, std::string port);

    void Connect();

    template <typename InfoT>
    InfoT ReceiveInfo() {
        return mSession->ReceiveInfo<InfoT>();
    }

    template <typename InfoT>
    void DeliverInfo(const InfoT &info) {
        mSession->DeliverInfo<InfoT>(info);
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