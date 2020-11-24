#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class IClient {
public:
    virtual void Connect() = 0;

    virtual void Reset() = 0;

    virtual void RegisterConnectCallback(const std::function<void()> &callback) = 0;

    virtual std::unique_ptr<Info> ReceiveInfo(const std::type_info *infoType) = 0;

    virtual void DeliverInfo(const std::type_info *infoType, const Info &info) = 0;
};

class Client : public IClient {
public:
    explicit Client(std::string host, std::string port);

    void Connect() override;
    
    void Reset() override;

    void RegisterConnectCallback(const std::function<void()> &callback) override {
        OnConnect = callback;
    }

    std::unique_ptr<Info> ReceiveInfo(const std::type_info *infoType) override;

    void DeliverInfo(const std::type_info *infoType, const Info &info) override;

private:
    template<typename InfoT>
    std::unique_ptr<InfoT> ReceiveInfoImpl() {
        return mSession->ReceiveInfo<InfoT>();
    }

    template<typename InfoT>
    void DeliverInfoImpl(const InfoT &info) {
        mSession->DeliverInfo<InfoT>(info);
    }

private:
    std::function<void()> OnConnect;

private:
    const std::string mHost;
    const std::string mPort;

    asio::io_context mContext;
    std::unique_ptr<Session> mSession;

    bool mShouldReset{true};
};
}}