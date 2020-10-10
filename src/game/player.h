#pragma once

#include <memory>
#include <set>

#include "stat.h"
#include "../network/client.h"
#include "../ui/ui_manager.h"

namespace UNO { namespace Game {

using namespace Network;
using namespace UI;

class Player {
public:
    explicit Player(std::string username, std::string host, std::string port);

private:
    void JoinGame();

    void GameLoop();

    void HandleDraw(const std::unique_ptr<DrawInfo> &info);
    
    void HandleSkip(const std::unique_ptr<SkipInfo> &info);
    
    void HandlePlay(const std::unique_ptr<PlayInfo> &info);

    void UpdateStateAfterPlay(int playerIndex, Card cardPlayed);

    void Win(int playerIndex);

    void PrintLocalState();

private:
    const std::string mUsername;
    Network::Client mClient;

    std::unique_ptr<HandCards> mHandCards;
    std::unique_ptr<UIManager> mUIManager;

    // state of game board
    std::unique_ptr<GameStat> mGameStat;

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}