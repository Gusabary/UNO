#pragma once

#include <array>
#include <cstring>
#include <cassert>

#include "../game/stat.h"
#include "view_formatter.h"

namespace UNO { namespace UI {

using namespace Game;

class View {
public:
    View();

    void Clear();

    void DrawOtherBox(int playerIndex, const GameStat &gameStat, const PlayerStat &playerStat);

    void DrawSelfBox(const GameStat &gameStat, const PlayerStat &playerStat,
        const HandCards &handcards, int cursorIndex);

    friend std::ostream& operator<<(std::ostream& os, const View& view);

private:
    void DrawBorderAndUsername(int row, int col, int width, int height, const std::string &username);

    void DrawHorizontalBorder(int row, int col, int length);

    void DrawVerticalBorder(int row, int col, int height);

    void Copy(int row, int col, const std::string &src);

private:
    using ViewT = std::array<std::array<char, 60>, 15>;

    constexpr static int OTHER_BOX_WIDTH = 22;
    constexpr static int OTHER_BOX_HEIGHT = 4;
    constexpr static int SELF_BOX_HEIGHT = 3;
    const static std::string CARDS_REMAINED_STR;
    const static std::string LAST_PLAYED_STR;
    const static std::string HAND_CARDS_STR;

    ViewT mView;
    ViewFormatter mFormatter;
};
}}