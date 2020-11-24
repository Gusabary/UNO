#pragma once

#include <memory>
#include <random>
#include <deque>
#include <cstdlib>

#include "stat.h"
#include "../network/server.h"

namespace UNO { namespace Game {

using namespace Network;

class GameBoard {
public:
    explicit GameBoard(std::shared_ptr<Network::IServer> serverSp);

    static std::shared_ptr<Network::IServer> CreateServer(const std::string &port);

    static std::pair<std::unique_ptr<DiscardPile>, std::unique_ptr<Deck>> CreateCardPile();

// private:
    void ReceiveUsername(int index, const std::string &username);

    void StartGame();

    void GameLoop();
    
    void HandleDraw(const std::unique_ptr<DrawInfo> &info);
    
    void HandleSkip(const std::unique_ptr<SkipInfo> &info);
    
    void HandlePlay(const std::unique_ptr<PlayInfo> &info);

    void Win();

    void ResetGame();

    template <typename ActionInfoT>
    void Broadcast(ActionInfoT &info) {
        int currentPlayer = mGameStat->GetCurrentPlayer();
        for (int i = 0; i < Common::Common::mPlayerNum; i++) {
            if (i != currentPlayer) {
                info.mPlayerIndex = Common::Util::WrapWithPlayerNum(currentPlayer - i);
                mServer->DeliverInfo(&typeid(ActionInfoT), i, info);
            }
        }
    }

public:
    // for tests
    // const std::unique_ptr<Network::IServer> &GetServer() const { return mServer; }

    const std::unique_ptr<DiscardPile> &GetDiscardPile() const { return mDiscardPile; }

    const std::unique_ptr<Deck> &GetDeck() const { return mDeck; }

    const std::unique_ptr<GameStat> &GetGameStat() const { return mGameStat; }

    const std::vector<PlayerStat> &GetPlayerStats() const { return mPlayerStats; }

private:
    std::shared_ptr<Network::IServer> mServer;

    // state of game board
    std::unique_ptr<DiscardPile> mDiscardPile;
    std::unique_ptr<Deck> mDeck;
    std::unique_ptr<GameStat> mGameStat;

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}