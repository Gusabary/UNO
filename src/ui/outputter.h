#pragma once

#include <string>

#include "view.h"

namespace UNO { namespace UI {

using namespace Game;

struct ColorEscape {
    const static std::string RESET;
    const static std::string RED;
    const static std::string YELLOW;
    const static std::string GREEN;
    const static std::string BLUE;
    // const static std::string BLACK;
};

class Outputter {
public:
    Outputter(std::unique_ptr<GameStat> &gameStat,
              std::vector<PlayerStat> &playerStats,
              std::unique_ptr<HandCards> &handCards);

    void PrintRawView(const View &view) const;

    void PrintView(const View &view) const;

    void PrintHintText(bool isSpecifyingNextColor, bool lastCardCanBePlayed,
        bool hasChanceToPlayAfterDraw) const;

private:
    std::vector<Card> GetCardsToRender() const;

    std::vector<ViewFormatter::PosT> GetPosesToRender() const;

    std::string ToColorEscape(CardColor color) const;

    void ClearScreen() const;

private:
    std::unique_ptr<GameStat> &mGameStat;
    std::vector<PlayerStat> &mPlayerStats;
    std::unique_ptr<HandCards> &mHandCards;
};
}}