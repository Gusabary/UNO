#pragma once

#include <asio.hpp>

#include "msg.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

class Session {
public:
    explicit Session(tcp::socket socket);

    template <typename MsgT>
    typename MsgT::InfoT ReceiveInfo() {
        Read();
        return reinterpret_cast<MsgT *>(mReadBuffer)->ToInfo();
    }

    template <typename MsgT>
    void DeliverInfo(const typename MsgT::InfoT &info) {
        reinterpret_cast<MsgT *>(mWriteBuffer)->FromInfo(info);
        Write();
    }

private:
    // read from mSocket to mReadBuffer
    void Read();

    // write from mWriteBuffer to mSocket
    void Write();

private:
    constexpr static int MAX_BUFFER_SIZE = 256;

    tcp::socket mSocket;
    uint8_t mReadBuffer[MAX_BUFFER_SIZE];
    uint8_t mWriteBuffer[MAX_BUFFER_SIZE];
};
}}