#pragma once

#include <memory>
#include <cassert>
#ifdef ENABLE_LOG
#include <spdlog/spdlog.h>
#endif

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

    /**
     * Consume a char with a timeout, if it's exceeded, an exception will be thrown
     *   \param milliseconds: timeout in milliseconds
     *   \param autoFlush: if true, the inputted char will get consumed regardless of '\n'
     *   \return the consumed char
     */
    static char GetCharWithTimeout(int milliseconds, bool autoFlush);

    /**
     * Get the number of segment, given the number of cards in hand.
     */
    static int GetSegmentNum(int handcardNum);

    /**
     * Get the index of segment that a card in hand belongs to.
     */
    static int GetSegmentIndex(int handcardIndex);

    /**
     * Get the index in a segment, given the index of the card in hand.
     */
    static int GetIndexInSegment(int handcardIndex);

    /**
     * Hide the terminal cursor for better player experience, 
     * especially when clean screen without using cls
     */
    static void HideTerminalCursor();

    /**
     * Dynamically cast a unique_ptr to one with another type.
     */
    template<typename DstInfoT, typename SrcInfoUp>
    static std::unique_ptr<DstInfoT> DynamicCast(SrcInfoUp &&srcInfo) {
        return std::unique_ptr<DstInfoT>(dynamic_cast<DstInfoT *>(srcInfo.release()));
    }

    /**
     * Helper of ReceiveInfo for brief code.
     */
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

    /**
     * Helper of DelieveInfo for brief code.
     */
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