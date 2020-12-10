#pragma once

#include <optional>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "common.h"

namespace UNO { namespace Common {

/**
 * config info that is used by \c GameBoard and \c Player, 
 * which is outside the \c Config class
 */
struct GameConfigInfo {
    bool mIsServer;
    std::string mHost;
    std::string mPort;
    std::string mUsername;
    std::string mLogPath{"logs/uno.log"};
};

/**
 * config info that is used by Config::SetUpCommonConfig,
 * which is inside the \c Config class
 */
struct CommonConfigInfo {
    std::optional<int> mPlayerNum;
    std::optional<std::string> mRedEscape;
    std::optional<std::string> mYellowEscape;
    std::optional<std::string> mGreenEscape;
    std::optional<std::string> mBlueEscape;
};

class Config {
public:
    Config(int argc, const char **argv);

    /**
     * Parse the config info from both yaml file and command line.
     *   \return config info that will be used outside the class
     */
    std::unique_ptr<GameConfigInfo> Parse();

private:
    /**
     * Handle immediate config in a short cut, like -v and -h.
     */
    void HandleImmediateConfig();
    /**
     * Parse the config info from yaml file.
     */
    void ParseFileOpts();

    /**
     * Parse the config info from command line.
     */
    void ParseCmdlineOpts();

    /**
     * Initialize variables in \c Common::Common with config info just parsed
     * and those variables are what will be actually used in the game. 
     */
    void SetUpCommonConfig();

private:
    std::unique_ptr<cxxopts::Options> mOptions;
    std::unique_ptr<cxxopts::ParseResult> mCmdlineOpts;
    std::unique_ptr<YAML::Node> mServerNode;
    std::unique_ptr<YAML::Node> mClientNode;

    std::unique_ptr<GameConfigInfo> mGameConfigInfo{std::make_unique<GameConfigInfo>()};
    std::unique_ptr<CommonConfigInfo> mCommonConfigInfo{std::make_unique<CommonConfigInfo>()};

private:
    const static std::string CMD_OPT_SHORT_LISTEN;
    const static std::string CMD_OPT_LONG_LISTEN;
    const static std::string CMD_OPT_BOTH_LISTEN;
    const static std::string CMD_OPT_SHORT_CONNECT;
    const static std::string CMD_OPT_LONG_CONNECT;
    const static std::string CMD_OPT_BOTH_CONNECT;
    const static std::string CMD_OPT_SHORT_USERNAME;
    const static std::string CMD_OPT_LONG_USERNAME;
    const static std::string CMD_OPT_BOTH_USERNAME;
    const static std::string CMD_OPT_SHORT_PLAYERS;
    const static std::string CMD_OPT_LONG_PLAYERS;
    const static std::string CMD_OPT_BOTH_PLAYERS;
    const static std::string CMD_OPT_SHORT_CFGFILE;
    const static std::string CMD_OPT_LONG_CFGFILE;
    const static std::string CMD_OPT_BOTH_CFGFILE;
    const static std::string CMD_OPT_LONG_LOGFILE;
    const static std::string CMD_OPT_SHORT_VERSION;
    const static std::string CMD_OPT_LONG_VERSION;
    const static std::string CMD_OPT_BOTH_VERSION;
    const static std::string CMD_OPT_SHORT_HELP;
    const static std::string CMD_OPT_LONG_HELP;
    const static std::string CMD_OPT_BOTH_HELP;

    const static std::string FILE_OPT_SERVER;
    const static std::string FILE_OPT_CLIENT;
    const static std::string FILE_OPT_LISTEN;
    const static std::string FILE_OPT_CONNECT;
    const static std::string FILE_OPT_USERNAME;
    const static std::string FILE_OPT_PLAYERS;
    const static std::string FILE_OPT_RED;
    const static std::string FILE_OPT_YELLOW;
    const static std::string FILE_OPT_GREEN;
    const static std::string FILE_OPT_BLUE;
};
}}