#include "config.h"

namespace UNO { namespace Common {

// define static common variables here
int Common::mPlayerNum;
int Common::mTimeoutPerTurn;
int Common::mHandCardsNumPerRow;
std::string Common::mRedEscape;
std::string Common::mYellowEscape;
std::string Common::mGreenEscape;
std::string Common::mBlueEscape;
const std::map<std::string, std::string> Common::mEscapeMap = {
    {"red",          "\033[31m"},
    {"yellow",       "\033[33m"},
    {"green",        "\033[32m"},
    {"blue",         "\033[34m"},
    {"cyan",         "\033[36m"},
    {"brightRed",    "\033[91m"},
    {"brightYellow", "\033[93m"},
    {"brightGreen",  "\033[92m"},
    {"brightBlue",   "\033[94m"},
    {"brightCyan",   "\033[96m"}
};

const std::string Config::CMD_OPT_SHORT_LISTEN = "l";
const std::string Config::CMD_OPT_LONG_LISTEN = "listen";
const std::string Config::CMD_OPT_BOTH_LISTEN = CMD_OPT_SHORT_LISTEN + ", " + CMD_OPT_LONG_LISTEN;
const std::string Config::CMD_OPT_SHORT_CONNECT = "c";
const std::string Config::CMD_OPT_LONG_CONNECT = "connect";
const std::string Config::CMD_OPT_BOTH_CONNECT = CMD_OPT_SHORT_CONNECT + ", " + CMD_OPT_LONG_CONNECT;
const std::string Config::CMD_OPT_SHORT_USERNAME = "u";
const std::string Config::CMD_OPT_LONG_USERNAME = "username";
const std::string Config::CMD_OPT_BOTH_USERNAME = CMD_OPT_SHORT_USERNAME + ", " + CMD_OPT_LONG_USERNAME;
const std::string Config::CMD_OPT_SHORT_PLAYERS = "n";
const std::string Config::CMD_OPT_LONG_PLAYERS = "players";
const std::string Config::CMD_OPT_BOTH_PLAYERS = CMD_OPT_SHORT_PLAYERS + ", " + CMD_OPT_LONG_PLAYERS;
const std::string Config::CMD_OPT_SHORT_CFGFILE = "f";
const std::string Config::CMD_OPT_LONG_CFGFILE = "file";
const std::string Config::CMD_OPT_BOTH_CFGFILE = CMD_OPT_SHORT_CFGFILE + ", " + CMD_OPT_LONG_CFGFILE;
const std::string Config::CMD_OPT_LONG_LOGFILE = "log";
const std::string Config::CMD_OPT_SHORT_VERSION = "v";
const std::string Config::CMD_OPT_LONG_VERSION = "version";
const std::string Config::CMD_OPT_BOTH_VERSION = CMD_OPT_SHORT_VERSION + ", " + CMD_OPT_LONG_VERSION;
const std::string Config::CMD_OPT_SHORT_HELP = "h";
const std::string Config::CMD_OPT_LONG_HELP = "help";
const std::string Config::CMD_OPT_BOTH_HELP = CMD_OPT_SHORT_HELP + ", " + CMD_OPT_LONG_HELP;
const std::string Config::FILE_OPT_SERVER = "server";
const std::string Config::FILE_OPT_CLIENT = "client";
const std::string Config::FILE_OPT_LISTEN = "listenOn";
const std::string Config::FILE_OPT_CONNECT = "connectTo";
const std::string Config::FILE_OPT_USERNAME = "username";
const std::string Config::FILE_OPT_PLAYERS = "playerNum";
const std::string Config::FILE_OPT_RED = "red";
const std::string Config::FILE_OPT_YELLOW = "yellow";
const std::string Config::FILE_OPT_GREEN = "green";
const std::string Config::FILE_OPT_BLUE = "blue";

Config::Config(int argc, const char **argv)
{
    mOptions = std::make_unique<cxxopts::Options>("uno", "UNO - uno card game");
    mOptions->add_options()
        (CMD_OPT_BOTH_LISTEN, "the port number that server will listen on", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_CONNECT, "the endpoint that client (player) will connect to", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_USERNAME, "the username of the player", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_PLAYERS, "the number of players", cxxopts::value<int>())
        (CMD_OPT_BOTH_CFGFILE, "the path of config file", cxxopts::value<std::string>())
        (CMD_OPT_LONG_LOGFILE, "the path of log file", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_VERSION, "show version of application", cxxopts::value<bool>())
        (CMD_OPT_BOTH_HELP, "show help info", cxxopts::value<bool>());
    
    try {
        mCmdlineOpts = std::make_unique<cxxopts::ParseResult>(mOptions->parse(argc, argv));
    }
    catch (std::exception &e) {
        std::cout << mOptions->help() << std::endl;
        std::cout << e.what() << std::endl;
        std::exit(-1);
    }
    std::string configFile;
    if (mCmdlineOpts->count(CMD_OPT_LONG_CFGFILE)) {
        auto configFile = (*mCmdlineOpts)[CMD_OPT_LONG_CFGFILE].as<std::string>();
        auto rootNode = YAML::LoadFile(configFile);
        if (rootNode[FILE_OPT_SERVER].IsDefined()) {
            mServerNode = std::make_unique<YAML::Node>(rootNode[FILE_OPT_SERVER]);
        }
        if (rootNode[FILE_OPT_CLIENT].IsDefined()) {
            mClientNode = std::make_unique<YAML::Node>(rootNode[FILE_OPT_CLIENT]);
        }
    }
}

std::unique_ptr<GameConfigInfo> Config::Parse()
{
    HandleImmediateConfig();
    // options from command line takes precedence over ones from config file
    ParseFileOpts();
    try {
        ParseCmdlineOpts();
    }
    catch (std::exception &e) {
        std::cout << mOptions->help() << std::endl;
        std::cout << e.what() << std::endl;
        std::exit(-1);
    }

    // handle common config here
    SetUpCommonConfig();

    // the main function will handle game config
    return std::move(mGameConfigInfo);
}

void Config::HandleImmediateConfig()
{
    if (mCmdlineOpts->count(CMD_OPT_LONG_HELP)) {
        std::cout << mOptions->help() << std::endl;
        std::exit(0);
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_VERSION)) {
        std::cout << "uno version 1.0" << std::endl;
        std::exit(0);
    }
}

void Config::ParseFileOpts()
{
    // parse server node
    if (mServerNode && mCmdlineOpts->count(CMD_OPT_LONG_LISTEN)) {
        if ((*mServerNode)[FILE_OPT_PLAYERS].IsDefined()) {
            mCommonConfigInfo->mPlayerNum = (*mServerNode)[FILE_OPT_PLAYERS].as<int>();
        }
    }

    // parse client node
    if (mClientNode && mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        if ((*mClientNode)[FILE_OPT_USERNAME].IsDefined()) {
            mGameConfigInfo->mUsername = (*mClientNode)[FILE_OPT_USERNAME].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_RED].IsDefined()) {
            mCommonConfigInfo->mRedEscape = (*mClientNode)[FILE_OPT_RED].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_YELLOW].IsDefined()) {
            mCommonConfigInfo->mYellowEscape = (*mClientNode)[FILE_OPT_YELLOW].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_GREEN].IsDefined()) {
            mCommonConfigInfo->mGreenEscape = (*mClientNode)[FILE_OPT_GREEN].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_BLUE].IsDefined()) {
            mCommonConfigInfo->mBlueEscape = (*mClientNode)[FILE_OPT_BLUE].as<std::string>();
        }
    }
}

void Config::ParseCmdlineOpts()
{
    // check options
    if (mCmdlineOpts->count(CMD_OPT_LONG_LISTEN) && mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        throw std::runtime_error("cannot specify both -l and -c options at the same time");
    }
    if (!mCmdlineOpts->count(CMD_OPT_LONG_LISTEN) && !mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        throw std::runtime_error("must specify either -l or -c option");
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT) && !mCmdlineOpts->count(CMD_OPT_LONG_USERNAME)
        && (!mClientNode || !(*mClientNode)[FILE_OPT_USERNAME].IsDefined())) {
        throw std::runtime_error("must specify -u option if -c option is specified");
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT) && mCmdlineOpts->count(CMD_OPT_LONG_PLAYERS)) {
        throw std::runtime_error("only server side can specify -n option");
    }

    // -l
    if (mCmdlineOpts->count(CMD_OPT_LONG_LISTEN)) {
        mGameConfigInfo->mIsServer = true;
        mGameConfigInfo->mPort = (*mCmdlineOpts)[CMD_OPT_LONG_LISTEN].as<std::string>();
    }

    // -c
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        mGameConfigInfo->mIsServer = false;
        std::string endpoint = (*mCmdlineOpts)[CMD_OPT_LONG_CONNECT].as<std::string>();
        int pos = endpoint.find(":");
        mGameConfigInfo->mHost = endpoint.substr(0, pos);
        mGameConfigInfo->mPort = endpoint.substr(pos + 1);
    }

    // -u
    if (mCmdlineOpts->count(CMD_OPT_LONG_USERNAME)) {
        mGameConfigInfo->mUsername = (*mCmdlineOpts)[CMD_OPT_LONG_USERNAME].as<std::string>();
    }

    // -n
    if (mCmdlineOpts->count(CMD_OPT_LONG_PLAYERS)) {
        mCommonConfigInfo->mPlayerNum = (*mCmdlineOpts)[CMD_OPT_LONG_PLAYERS].as<int>();
    }

    // --log
    if (mCmdlineOpts->count(CMD_OPT_LONG_LOGFILE)) {
        mGameConfigInfo->mLogPath = (*mCmdlineOpts)[CMD_OPT_LONG_LOGFILE].as<std::string>();
    }
}

void Config::SetUpCommonConfig()
{
    Common::mPlayerNum = mCommonConfigInfo->mPlayerNum.value_or(3);
    Common::mTimeoutPerTurn = 15;
    Common::mHandCardsNumPerRow = 8;
    
    auto redIter = Common::mEscapeMap.find(mCommonConfigInfo->mRedEscape.value_or("red"));
    if (redIter == Common::mEscapeMap.end()) {
        Common::mRedEscape = Common::mEscapeMap.at("red");
    }
    else {
        Common::mRedEscape = redIter->second;
    }

    auto yellowIter = Common::mEscapeMap.find(mCommonConfigInfo->mYellowEscape.value_or("yellow"));
    if (yellowIter == Common::mEscapeMap.end()) {
        Common::mYellowEscape = Common::mEscapeMap.at("yellow");
    }
    else {
        Common::mYellowEscape = yellowIter->second;
    }

    auto greenIter = Common::mEscapeMap.find(mCommonConfigInfo->mGreenEscape.value_or("green"));
    if (greenIter == Common::mEscapeMap.end()) {
        Common::mGreenEscape = Common::mEscapeMap.at("green");
    }
    else {
        Common::mGreenEscape = greenIter->second;
    }

    auto blueIter = Common::mEscapeMap.find(mCommonConfigInfo->mBlueEscape.value_or("blue"));
    if (blueIter == Common::mEscapeMap.end()) {
        Common::mBlueEscape = Common::mEscapeMap.at("blue");
    }
    else {
        Common::mBlueEscape = blueIter->second;
    }
}
}}