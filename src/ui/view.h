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

    /**
     * Clear the view. If \param doClearIndicator is true, 
     * do not clear the line where \param currentPlayer 's time indicator positioned.
     */
    void Clear(bool doClearIndicator, int currentPlayer = 0);

    /**
     * Draw the whole view when waiting for other players to join.
     */
    void DrawWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime);

    /**
     * Draw a box of other player.
     */
    void DrawOtherBox(int playerIndex, const GameStat &gameStat, const PlayerStat &playerStat);

    /**
     * Draw the box of player himself.
     */
    void DrawSelfBox(const GameStat &gameStat, const PlayerStat &playerStat,
        const HandCards &handcards, int cursorIndex);

    /**
     * Draw the last played card near the center of game board.
     */
    void DrawLastPlayedCard(Card lastPlayedCard);

    /**
     * Draw the time indicator of \param currentPlayer, which has passed \param timeElapsed seconds.
     */
    void DrawTimeIndicator(int currentPlayer, int timeElapsed);

    /**
     * Make sure that if it's my turn, there must be a self time indicator.
     * i.e. self time indicator and hint text should always appear at the same time.
     */
    void DrawSelfTimeIndicatorIfNot();

    /**
     * The handcard may occupy more than one row, in which condition, there is an extra row number.
     */
    int GetExtraRowNum() const { return mExtraRowNum; }

    /**
     * Get the char in view positioned at \param row in row and \param col in column.
     */
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