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

    void DrawWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime);

    void DrawOtherBox(int playerIndex, const GameStat &gameStat, const PlayerStat &playerStat);

    void DrawSelfBox(const GameStat &gameStat, const PlayerStat &playerStat,
        const HandCards &handcards, int cursorIndex);

    void DrawLastPlayedCard(Card lastPlayedCard);

    void DrawTimeIndicator(int currentPlayer, int timeElapsed);
    
    int GetExtraRowNum() const { return mExtraRowNum; }

    char At(int row, int col) const { return mView[row][col]; }

    friend std::ostream& operator<<(std::ostream& os, const View& view);

private:
    void DrawBorder(int row, int col, int width, int height);
    
    void DrawBorderAndUsername(int row, int col, int width, int height, const std::string &username);

    void DrawHorizontalBorder(int row, int col, int length);

    void DrawVerticalBorder(int row, int col, int height);

    void DrawHandCards(int row, int col, int width, const HandCards &handcards);

    void DrawUNO();

    int GetSelfBoxHeight();

    void Copy(int row, int col, const std::string &src);

    void AlignCenter(int row, int col, int width, const std::string &src);

private:
    using ViewT = std::vector<std::vector<char>>;

    const static std::string CARDS_REMAINED_STR;
    const static std::string LAST_PLAYED_STR;
    const static std::string HAND_CARDS_STR;
    const static std::string UNO_STR;

    ViewT mView;

    int mExtraRowNum{0};  // due to more than one handcard segment
    int mMyIndex;
};
}}