#include "view.h"

namespace UNO { namespace UI {

const std::string View::CARDS_REMAINED_STR = "cards remained: ";
const std::string View::LAST_PLAYED_STR = "last played: ";
const std::string View::HAND_CARDS_STR = "handcards: ";

View::View()
{
    Clear();
}

void View::Clear()
{
    for (auto &row : mView) {
        for (auto &c : row) {
            c = ' ';
        }
    }
}

/**
 * draw a box of other player (instead of the player himself), whose top left corner is
 * located by \p row and \p col, and contains the information of \p gameStat and \p playerStat.
 * if \p isCurrentPlayer is true, a star will be marked on the top right corner of the box.
 * the other box looks like:
 *      +--------------------+
 *      | Gusabary         * |
 *      +--------------------+
 *      | cards remained: 7  |
 *      | last played: YR    |
 *      +--------------------+
 */
void View::DrawOtherBox(int row, int col, const GameStat &gameStat, const PlayerStat &playerStat,
    bool isCurrentPlayer)
{
    DrawBorderAndUsername(row, col, OTHER_BOX_WIDTH, OTHER_BOX_HEIGHT, playerStat.GetUsername());
    if (isCurrentPlayer) {
        mView[row + 1][col + OTHER_BOX_WIDTH - 3] = '*';
    }

    Copy(row + 3, col + 2, CARDS_REMAINED_STR);
    Copy(row + 3, col + 2 + CARDS_REMAINED_STR.size(), std::to_string(playerStat.GetRemainingHandCardsNum()));

    Copy(row + 4, col + 2, LAST_PLAYED_STR);
    if (playerStat.DoPlayInLastRound()) {
        Copy(row + 4, col + 2 + LAST_PLAYED_STR.size(), playerStat.GetLastPlayedCard().ToString());
    }
}

/**
 * draw a box of the player himself, whose top left corner is located by \p row and \p col,
 * and contains the information of \p gameStat, \p playerStat and \p handcards.
 * the self box looks like:
 *      +----------------------------+
 *      | Gusabary                 * |
 *      +----------------------------+
 *      | R1  R2  Y3  R4  W >B+2  +4 |
 *      +----------------------------+
 */
void View::DrawSelfBox(int row, int col, const GameStat &gameStat, const PlayerStat &playerStat, 
    const HandCards &handcards, int cursorIndex)
{
    int width = 1 + handcards.Length() + 1;
    DrawBorderAndUsername(row, col, width, SELF_BOX_HEIGHT, playerStat.GetUsername());
    Copy(row + 3, col + 1, handcards.ToString());

    if (gameStat.IsMyTurn()) {
        mView[row + 1][col + width - 3] = '*';
        // show cursor only in the turn of player himself
        int cursorPos = col + 1 + handcards.LengthBeforeIndex(cursorIndex);
        mView[row + 3][cursorPos] = '>';
    }
}

/**
 * draw the border and username of a box, whose top left corner is located by \p row and \p col,
 * with width (including '+' at both sides) of \p width, height (not including '+' at both sides) 
 * of \p height and username of \p username.
 */
void View::DrawBorderAndUsername(int row, int col, int width, int height, const std::string &username)
{
    DrawHorizontalBorder(row, col, width);
    DrawHorizontalBorder(row + height + 1, col, width);
    DrawVerticalBorder(row + 1, col, height);
    DrawVerticalBorder(row + 1, col + width - 1, height);
    DrawHorizontalBorder(row + 2, col, width);
    Copy(row + 1, col + 2, username);
}

/**
 * draw a horizontal border which starts at the position located by \p row and \p col,
 * and has a length of \p length, including '+' at both sides
 */
void View::DrawHorizontalBorder(int row, int col, int length)
{
    assert(length > 2);
    std::string border = "++";
    border.insert(1, length - 2, '-');
    Copy(row, col, border);
}

/**
 * draw a vertical border which starts at the position located by \p row and \p col,
 * and has a height of \p height, not including '+' at both sides
 */
void View::DrawVerticalBorder(int row, int col, int height)
{
    std::cout << "***" << col << std::endl;
    for (int i = 0; i < height; i++) {
        mView[row + i][col] = '|';
    }
}

/**
 * copy the string \p src to the position in view located by \p row and \p col.
 * note that we use `strncpy` here instead of `strcpy`, that is because `strcpy`
 * copys the terminated-null too, which will cover the space following the string.
 * with `strncpy`, we can copy the string without the terminated-null.
 */
void View::Copy(int row, int col, const std::string &src)
{
    std::strncpy(mView[row].data() + col, src.c_str(), src.size());
}

std::ostream& operator<<(std::ostream& os, const View& view)
{
    for (auto row : view.mView) {
        for (auto c : row) {
            os << c;
        }
        os << std::endl;
    }
    return os;
}

}}