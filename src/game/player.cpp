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
    mClient.DeliverInfo<JoinGameInfo>(JoinGameInfo(mUsername));

    // wait for game start
    GameStartInfo info = mClient.ReceiveInfo<GameStartInfo>();
    std::cout << "game start info received: " << std::endl << info << std::endl;
}
}}