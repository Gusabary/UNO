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
};
}}