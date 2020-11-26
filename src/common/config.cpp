#include "config.h"

namespace UNO { namespace Common {

// define static common variables here
int Common::Common::mPlayerNum;
int Common::Common::mTimeoutPerTurn;
int Common::Common::mHandCardsNumPerRow;

Config::Config(int argc, const char **argv)
{
    mOptions = std::make_unique<cxxopts::Options>("uno", "UNO - uno card game");
    mOptions->add_options()
        ("l, listen", "the port number that server will listen on", cxxopts::value<std::string>())
        ("c, connect", "the endpoint that client (player) will connect to", cxxopts::value<std::string>())
        ("u, username", "the username of the player", cxxopts::value<std::string>())
        ("n, players", "the number of players", cxxopts::value<int>())
        ("f, file", "the path of config file", cxxopts::value<std::string>());
    
    mCmdlineOpts = std::make_unique<cxxopts::ParseResult>(mOptions->parse(argc, argv));
    if (mCmdlineOpts->count("file")) {
        auto configFile = (*mCmdlineOpts)["file"].as<std::string>();
        auto rootNode = YAML::LoadFile(configFile);
        if (rootNode["server"].IsDefined()) {
            mServerNode = std::make_unique<YAML::Node>(rootNode["server"]);
        }
        if (rootNode["client"].IsDefined()) {
            mClientNode = std::make_unique<YAML::Node>(rootNode["client"]);
        }
    }
}

std::unique_ptr<GameConfigInfo> Config::Parse()
{
    // options from command line takes precedence over ones from config file
    ParseFileOpts();
    try {
        ParseCmdlineOpts();
    }
    catch (std::exception &e) {
        std::cout << mOptions->help() << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
    }

    // handle common config here
    SetUpCommonConfig();

    // the main function will handle game config
    return std::move(mGameConfigInfo);
}

void Config::ParseFileOpts()
{
    // parse server node
    if (mServerNode && mCmdlineOpts->count("listen")) {
        if ((*mServerNode)["playerNum"].IsDefined()) {
            mCommonConfigInfo->mPlayerNum = (*mServerNode)["playerNum"].as<int>();
        }
    }

    // parse client node
    if (mClientNode && mCmdlineOpts->count("connect")) {

    }
}

void Config::ParseCmdlineOpts()
{
    // check options
    if (mCmdlineOpts->count("listen") && mCmdlineOpts->count("connect")) {
        throw std::runtime_error("cannot specify both -l and -c options at the same time");
    }
    if (!mCmdlineOpts->count("listen") && !mCmdlineOpts->count("connect")) {
        throw std::runtime_error("must specify either -l or -c option");
    }
    if (mCmdlineOpts->count("connect") && !mCmdlineOpts->count("username")) {
        throw std::runtime_error("must specify -u option if -c option is specified");
    }
    if (mCmdlineOpts->count("connect") && mCmdlineOpts->count("players")) {
        throw std::runtime_error("only server side can specify -n option");
    }

    // -l
    if (mCmdlineOpts->count("listen")) {
        mGameConfigInfo->mIsServer = true;
        // mGameConfigInfo->mHost = "localhost";
        mGameConfigInfo->mPort = (*mCmdlineOpts)["listen"].as<std::string>();
    }

    // -c
    if (mCmdlineOpts->count("connect")) {
        mGameConfigInfo->mIsServer = false;
        std::string endpoint = (*mCmdlineOpts)["connect"].as<std::string>();
        int pos = endpoint.find(":");
        mGameConfigInfo->mHost = endpoint.substr(0, pos);
        mGameConfigInfo->mPort = endpoint.substr(pos + 1);
    }

    // -u
    if (mCmdlineOpts->count("username")) {
        mGameConfigInfo->mUsername = (*mCmdlineOpts)["username"].as<std::string>();
    }

    // -n
    if (mCmdlineOpts->count("players")) {
        mCommonConfigInfo->mPlayerNum = (*mCmdlineOpts)["players"].as<int>();
    }
}

void Config::SetUpCommonConfig()
{
    Common::Common::mPlayerNum = mCommonConfigInfo->mPlayerNum.value_or(3);
    Common::Common::mTimeoutPerTurn = 15;
    Common::Common::mHandCardsNumPerRow = 8;
}
}}