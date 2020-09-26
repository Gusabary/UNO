#pragma once

#include <memory>

#include "../network/server.h"

namespace UNO { namespace Game {

class GameBoard {
public:
    explicit GameBoard(std::string port);

private:
    void ReceiveUsername(int index, const std::string &username);

private:
    Network::Server mServer;
};
}}