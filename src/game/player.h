#pragma once

#include <memory>
#include <set>
#include <type_traits>

#include "stat.h"
#include "../network/client.h"
#include "../ui/ui_manager.h"

namespace UNO { namespace Game {

using namespace Network;
using namespace UI;

class Player {
public:
    explicit Player(std::string username, std::shared_ptr<Network::IClient> clientSp);

    void Start();

    static std::shared_ptr<Network::IClient> CreateClient(const std::string &host, 
        const std::string &port);

private:
    /**
     * Connection succeeded. Prepare game state and wait for others joining.
     */
    void JoinGame();

    /**
     * Main game loop, 
     *   1) in player's turn, get his action and deliver to server;
     *   2) in others' turn, receive info and render ui.
     */
    void GameLoop();

    /**
     * Deliver info after player does a draw action.
     */
    void HandleSelfDraw();

    /**
     * Deliver info after player does a skip action.
     */
    void HandleSelfSkip();

    /**
     * Deliver info after player does a play action.
     */
    bool HandleSelfPlay(int cardIndex);

    /**
     * Update state after a draw action either from player or others.
     */
    void UpdateStateAfterDraw(int playerIndex, int number, int indexOfNewlyDrawn = -1);
    
     /**
     * Update state after a skip action either from player or others.
     */
    void UpdateStateAfterSkip(int playerIndex);
    
     /**
     * Update state after a play action either from player or others.
     */
    void UpdateStateAfterPlay(int playerIndex, Card cardPlayed);

    /**
     * Someone has won, end game.
     */
    void Win(int playerIndex);

    /**
     * Wait for the player to decide whether want to play again.
     */
    void GameEnds();

    /**
     * Reset the game state and prepare for reconnection.
     */
    void ResetGame();

    void PrintLocalState();

private:
    const std::string mUsername;
    std::string mWinner;
    std::shared_ptr<Network::IClient> mClient;

    std::unique_ptr<UIManager> mUIManager;
    std::unique_ptr<HandCards> mHandCards;

    // state of game board
    std::unique_ptr<GameStat> mGameStat;

    // state of all players
    std::vector<PlayerStat> mPlayerStats;
};
}}