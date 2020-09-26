#include "game_board.h"

namespace UNO { namespace Game {

GameBoard::GameBoard(std::string port)
    : mServer(port) 
{
    mServer.OnReceiveJoinGameInfo = [this](int index, const JoinGameInfo &info) {
        ReceiveUsername(index, info.mUsername);
    };

    mServer.Run();
}

void GameBoard::ReceiveUsername(int index, const std::string &username)
{
    std::cout << "receive, index: " << index << ", username: " << username << std::endl;
    GameStartInfo info;
    info.mFirstPlayer = 999;
    mServer.DeliverInfo<GameStartInfo>(index, info);
}

}}