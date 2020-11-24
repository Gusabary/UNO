#pragma once

#include <memory>
#include <cassert>
#include <spdlog/spdlog.h>

#include "common.h"
#include "terminal.h"

namespace UNO { 

namespace Network {
    
class IServer;

class IClient;

}

namespace Common {

class Util {
public:
    static int Wrap(int numToWrap, int range);

    static int WrapWithPlayerNum(int numToWrap);

    static char GetCharImmediately();

    static char GetCharWithTimeout(int milliseconds, bool autoFlush);

    static int GetSegmentNum(int handcardNum);

    static int GetSegmentIndex(int handcardIndex);

    static int GetIndexInSegment(int handcardIndex);

    template<typename DstInfoT, typename SrcInfoUp>
    static std::unique_ptr<DstInfoT> DynamicCast(SrcInfoUp &&srcInfo) {
        return std::unique_ptr<DstInfoT>(dynamic_cast<DstInfoT *>(srcInfo.release()));
    }

    template<typename InfoT, typename Peer, typename... Args>
    static std::unique_ptr<InfoT> Receive(std::shared_ptr<Peer> peer, Args... args) {
        if constexpr (std::is_same_v<Peer, Network::IServer>) {
            static_assert(sizeof...(args) == 1);
            return ReceiveHelper<InfoT>(peer, args...);
        }
        else if constexpr (std::is_same_v<Peer, Network::IClient>) {
            static_assert(sizeof...(args) == 0);
            return DynamicCast<InfoT>(peer->ReceiveInfo(&typeid(InfoT)));
        }
        else {
            assert(0);
        }
    }

    template<typename InfoT, typename Peer, typename... Args>
    static void Deliver(std::shared_ptr<Peer> peer, Args... args) {
        if constexpr (std::is_same_v<Peer, Network::IServer>) {
            DeliverHelper<InfoT>(peer, args...);
        }
        else if constexpr (std::is_same_v<Peer, Network::IClient>) {
            peer->DeliverInfo(&typeid(InfoT), InfoT{args...});
        }
        else {
            assert(0);
        }
    }

private:
    template<typename InfoT, typename Peer>
    static std::unique_ptr<InfoT> ReceiveHelper(std::shared_ptr<Peer> server, int index) {
        return DynamicCast<InfoT>(server->ReceiveInfo(&typeid(InfoT), index));
    }

    template<typename InfoT, typename Peer, typename... Args>
    static void DeliverHelper(std::shared_ptr<Peer> server, int index, Args... args) {
        server->DeliverInfo(&typeid(InfoT), index, InfoT{args...});
    }
};
}}