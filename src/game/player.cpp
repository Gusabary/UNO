#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::string host, std::string port)
    : mUsername(username), mClient(host, port) 
{
    mClient.OnConnect = [this]() { JoinGame(); };

    mClient.Connect();
}

void Player::JoinGame()
{
    std::cout << "connect success, sending username to server" << std::endl;
    mClient.DeliverJoinGameInfo(JoinGameInfo(mUsername));
    GameStartInfo info = mClient.ReceiveGameStartInfo();
    std::cout << "game start info received: " << info.mFirstPlayer << std::endl;
}
}}