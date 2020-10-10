#pragma once

#include <vector>

#include "../game/player_stat.h"

namespace UNO { namespace UI {

using namespace Game;

class UIManager {
public:
    UIManager(std::vector<PlayerStat> &playerStats) 
        : mPlayerStats(playerStats) {}

    void Render();
    
private:
    std::vector<PlayerStat> &mPlayerStats;
};
}}