#pragma once

#include <memory>

#include "client.h"

namespace UNO { namespace Game {

class Player {
public:
    explicit Player(std::string username, std::string host, std::string port);

private:

private:
    std::string mUsername;
    Network::Client mClient;
};
}}