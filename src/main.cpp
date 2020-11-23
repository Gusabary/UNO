#include <iostream>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "network/server.h"
#include "network/client.h"
#include "game/game_board.h"
#include "game/player.h"
#include "common/util.h"
#include "common/config.h"

using namespace UNO;

int main(int argc, char **argv)
{
    auto configInfo = Common::Config(argc, const_cast<const char **>(argv)).Parse();

    if (configInfo->mIsServer) {
        auto serverUp = Game::GameBoard::CreateServer(configInfo->mPort);
        Game::GameBoard gameBoard(serverUp);
    }
    else {
        auto clientUp = Game::Player::CreateClient(configInfo->mHost, configInfo->mPort);
        Game::Player player(configInfo->mUsername, clientUp);
    }

    return 0;
}