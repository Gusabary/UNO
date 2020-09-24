#include <iostream>
#include <cxxopts.hpp>

#include "server.h"
#include "client.h"
#include "player.h"

using namespace UNO;

struct ArgInfo {
    bool mIsServer;
    std::string mHost;
    std::string mPort;
    std::string mUsername;
};

ArgInfo parseArgs(int argc, const char **argv) 
{
    cxxopts::Options options("uno", "uno card game");
    options.add_options()
        ("l, listen", "the port number that server will listen on", cxxopts::value<std::string>())
        ("c, connect", "the endpoint that client will connect to", cxxopts::value<std::string>())
        ("u, username", "the username of the player", cxxopts::value<std::string>());

    ArgInfo argInfo;
    try {
        auto result = options.parse(argc, argv);

        // check options
        if (result.count("listen") && result.count("connect")) {
            throw std::runtime_error("cannot specify both -l and -c options at the same time");
        }
        if (!result.count("listen") && !result.count("connect")) {
            throw std::runtime_error("must specify either -l or -c option");
        }
        if (!result.count("username")) {
            throw std::runtime_error("must specify -u option");
        }

        // -l
        if (result.count("listen")) {
            argInfo.mIsServer = true;
            // argInfo.mHost = "localhost";
            argInfo.mPort = result["listen"].as<std::string>();
        }

        // -c
        if (result.count("connect")) {
            argInfo.mIsServer = false;
            std::string endpoint = result["connect"].as<std::string>();
            int pos = endpoint.find(":");
            argInfo.mHost = endpoint.substr(0, pos);
            argInfo.mPort = endpoint.substr(pos + 1);
        }

        // -u
        argInfo.mUsername = result["username"].as<std::string>();
    }
    catch (std::exception &e) {
        std::cout << options.help() << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
    }

    return argInfo;
}

int main(int argc, char **argv)
{
    const ArgInfo argInfo = parseArgs(argc, const_cast<const char **>(argv));

    if (argInfo.mIsServer) {
        Network::Server server(argInfo.mPort);
    }
    else {
        Game::Player player(argInfo.mUsername, argInfo.mHost, argInfo.mPort);
    }

    return 0;
}