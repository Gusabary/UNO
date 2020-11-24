#pragma once

#include <memory>
#include <asio.hpp>

#include "session.h"

namespace UNO { namespace Network {

using asio::ip::tcp;
using namespace Game;

class IServer {
public:
    virtual void Run() = 0;

    virtual void Close() = 0;

    virtual void Reset() = 0;

    virtual void RegisterReceiveJoinGameInfoCallback(
        const std::function<void(int, const JoinGameInfo &)> &callback) = 0;

    virtual void RegisterAllPlayersJoinedCallback(const std::function<void()> &callback) = 0;

    virtual std::unique_ptr<Info> ReceiveInfo(const std::type_info *infoType, int index) = 0;

    virtual void DeliverInfo(const std::type_info *infoType, int index, const Info &info) = 0;
};

class Server : public IServer {
public:
    explicit Server(std::string port);

    void Run() override;

    void Close() override;

    void Reset() override;

    void RegisterReceiveJoinGameInfoCallback(
        const std::function<void(int, const JoinGameInfo &)> &callback) override {
        OnReceiveJoinGameInfo = callback;
    }

    void RegisterAllPlayersJoinedCallback(const std::function<void()> &callback) override {
        OnAllPlayersJoined = callback;
    }

    std::unique_ptr<Info> ReceiveInfo(const std::type_info *infoType, int index) override;

    void DeliverInfo(const std::type_info *infoType, int index, const Info &info) override;

private:
    void Accept();

    template<typename InfoT>
    std::unique_ptr<InfoT> ReceiveInfoImpl(int index) {
        return mSessions[index]->ReceiveInfo<InfoT>();
    }

    template<typename InfoT>
    void DeliverInfoImpl(int index, const InfoT &info) {
        mSessions[index]->DeliverInfo<InfoT>(info);
    }

private:
    // callbacks in server side should always take index of session as the first parameter
    std::function<void(int, const JoinGameInfo &)> OnReceiveJoinGameInfo;

    std::function<void()> OnAllPlayersJoined;

private:
    const std::string mPort;
    
    asio::io_context mContext;
    std::unique_ptr<tcp::acceptor> mAcceptor;
    std::vector<std::unique_ptr<Session>> mSessions;

    bool mShouldReset{true};
};
}}