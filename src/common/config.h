#pragma once

#include <optional>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "common.h"

namespace UNO { namespace Common {

struct GameConfigInfo {
    bool mIsServer;
    std::string mHost;
    std::string mPort;
    std::string mUsername;
};

struct CommonConfigInfo {
    std::optional<int> mPlayerNum;
};

class Config {
public:
    Config(int argc, const char **argv);

    std::unique_ptr<GameConfigInfo> Parse();

private:
    void ParseFileOpts();

    void ParseCmdlineOpts();

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

    const static std::string FILE_OPT_SERVER;
    const static std::string FILE_OPT_CLIENT;
    const static std::string FILE_OPT_LISTEN;
    const static std::string FILE_OPT_CONNECT;
    const static std::string FILE_OPT_USERNAME;
    const static std::string FILE_OPT_PLAYERS;
};
}}