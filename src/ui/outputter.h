#pragma once

#include <string>

#include "view.h"

namespace UNO { namespace UI {

using namespace Game;

struct ColorEscape {
    static std::string RESET;
    static std::string RED;
    static std::string YELLOW;
    static std::string GREEN;
    static std::string BLUE;
    // const static std::string BLACK;
};

class Outputter {
public:
    Outputter(std::unique_ptr<GameStat> &gameStat,
              std::vector<PlayerStat> &playerStats,
              std::unique_ptr<HandCards> &handCards);

    /**
     * Print view without color rendering. 
     * Used when waiting for other playes to join.
     */
    void PrintRawView(const View &view) const;

    /**
     * Print view with color rendering.
     */
    void PrintView(const View &view, bool useCls = true) const;

    /**
     * Print the hint text shown below the view. Only appear in player's turn.
     */
    void PrintHintText(bool isSpecifyingNextColor, bool lastCardCanBePlayed,
        bool hasChanceToPlayAfterDraw) const;

private:
    std::vector<RenderInfo> GetRenderInfos() const;

    std::string ToColorEscape(CardColor color) const;

    void ClearScreen(bool useCls = true) const;

private:
    std::unique_ptr<GameStat> &mGameStat;
    std::vector<PlayerStat> &mPlayerStats;
    std::unique_ptr<HandCards> &mHandCards;
};
}}