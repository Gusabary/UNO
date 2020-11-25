#include "session.h"

namespace UNO { namespace Network {

Session::Session(tcp::socket socket) : mSocket(std::move(socket)) {}

/**
 * Read will throw end-of-file exception if the corresponding client has disconnected
 */
void Session::Read()
{
    std::memset(mReadBuffer, 0, MAX_BUFFER_SIZE);
    // read header
    asio::read(mSocket, asio::buffer(mReadBuffer, sizeof(Msg)));

    // read body
    int len = reinterpret_cast<Msg *>(mReadBuffer)->mLen;
    asio::read(mSocket, asio::buffer(mReadBuffer + sizeof(Msg), len));
}

void Session::Write() 
{
    Msg *msg = reinterpret_cast<Msg *>(mWriteBuffer);
    int len = sizeof(Msg) + msg->mLen;
    asio::async_write(mSocket, asio::buffer(msg, len), 
        [this](std::error_code, std::size_t) {
            std::memset(mWriteBuffer, 0, MAX_BUFFER_SIZE);
        }
    );
}

}}