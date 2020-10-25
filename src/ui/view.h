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

    void Clear(bool doClearIndicator, int currentPlayer = 0);

    void DrawOtherBox(int playerIndex, const GameStat &gameStat, const PlayerStat &playerStat);

    void DrawSelfBox(const GameStat &gameStat, const PlayerStat &playerStat,
        const HandCards &handcards, int cursorIndex);

    void DrawLastPlayedCard(Card lastPlayedCard);

    void DrawTimeIndicator(int currentPlayer, int timeElapsed);

    friend std::ostream& operator<<(std::ostream& os, const View& view);

private:
    void DrawBorderAndUsername(int row, int col, int width, int height, const std::string &username);

    void DrawHorizontalBorder(int row, int col, int length);

    void DrawVerticalBorder(int row, int col, int height);

    void DrawHandCards(int row, int col, const HandCards &handcards);

    int GetSelfBoxHeight();

    void Copy(int row, int col, const std::string &src);

    void AlignCenter(int row, int col, int width, const std::string &src);

    int GetSegmentNum(int handcardNum) const { return (handcardNum - 1) / 8 + 1; }

    int GetSegmentIndex(int handcardIndex) const { return handcardIndex / 8; }

    int GetIndexInSegment(int handcardIndex) const { return handcardIndex % 8; }

private:
    using ViewT = std::vector<std::vector<char>>;

    constexpr static int OTHER_BOX_WIDTH = 22;
    constexpr static int OTHER_BOX_HEIGHT = 4;
    constexpr static int SELF_BOX_WIDTH = 42;
    constexpr static int SELF_BOX_HEIGHT_BASE = 3;
    const static std::string CARDS_REMAINED_STR;
    const static std::string LAST_PLAYED_STR;
    const static std::string HAND_CARDS_STR;

    ViewT mView;
    ViewFormatter mFormatter;

    int mExtraRowNum;  // due to more than one handcard segment
};
}}