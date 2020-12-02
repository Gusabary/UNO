#include "view.h"

namespace UNO { namespace UI {

const std::string View::CARDS_REMAINED_STR = "cards remained: ";
const std::string View::LAST_PLAYED_STR = "last played: ";
const std::string View::HAND_CARDS_STR = "handcards: ";
const std::string View::UNO_STR = "UNO!";

View::View()
{
    auto [height, width] = ViewFormatter::GetMaxScaleOfView();
    mView.resize(height);
    for (auto &row : mView) {
        row.resize(width);
    }

    Clear(true);
}

void View::Clear(bool doClearIndicator, int currentPlayer)
{
    int rowNumNotToClear = -1;
    if (!doClearIndicator) {
        if (currentPlayer != 0) {
            rowNumNotToClear = ViewFormatter::GetPosOfPlayerBox(currentPlayer).first + 6;
        }
        else {
            rowNumNotToClear = ViewFormatter::GetPosOfPlayerBox(0).first + 5 + mExtraRowNum;
        }
    }

    for (int row = 0; row < mView.size(); row++) {
        if (row != rowNumNotToClear) {
            for (auto &c : mView[row]) {
                c = ' ';
            }
        }
    }
}

void View::DrawWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime)
{
    if (isFirstTime) {
        mMyIndex = usernames.size() - 1;
    }
    for (int playerIndex = 0; playerIndex < Common::Common::mPlayerNum; playerIndex++) {
        auto [row, col] = ViewFormatter::GetPosOfPlayerBox(playerIndex);
        auto [height, width] = ViewFormatter::GetBaseScaleOfBox(playerIndex);
        int curNum = usernames.size();
        int absoluteIndex = Common::Util::WrapWithPlayerNum(playerIndex + mMyIndex);
        if (absoluteIndex >= curNum) {
            // username unknown yet
            DrawBorder(row, col, width, height - 2);
        }
        else {
            DrawBorderAndUsername(row, col, width, height - 2, usernames[absoluteIndex]);
        }
        if (playerIndex == 0) {
            AlignCenter(row + 3, col, width, "Waiting for players to join...");
        }
        else {
            Copy(row + 3, col + 2, CARDS_REMAINED_STR);
            Copy(row + 4, col + 2, LAST_PLAYED_STR);
        }
    }
}

void View::DrawOtherBox(int playerIndex, const GameStat &gameStat, const PlayerStat &playerStat)
{
    bool isCurrentPlayer = false;
    auto [row, col] = ViewFormatter::GetPosOfPlayerBox(playerIndex);
    auto [height, width] = ViewFormatter::GetBaseScaleOfBox(playerIndex);
    DrawBorderAndUsername(row, col, width, height - 2, playerStat.GetUsername());
    if (gameStat.GetCurrentPlayer() == playerIndex) {
        isCurrentPlayer = true;
    }
    // if (isCurrentPlayer) {
        // mView[row + 1][col + OTHER_BOX_WIDTH - 3] = '*';
    // }

    Copy(row + 3, col + 2, CARDS_REMAINED_STR);
    auto remainingHandCardsNum = playerStat.GetRemainingHandCardsNum();
    Copy(row + 3, col + 2 + CARDS_REMAINED_STR.size(), std::to_string(remainingHandCardsNum));
    if (remainingHandCardsNum == 1) {
        DrawUNO();
    }

    Copy(row + 4, col + 2, LAST_PLAYED_STR);
    if (playerStat.DoPlayInLastRound()) {
        Copy(row + 4, col + 2 + LAST_PLAYED_STR.size(), playerStat.GetLastPlayedCard().ToString());
    }
}

void View::DrawSelfBox(const GameStat &gameStat, const PlayerStat &playerStat, 
    const HandCards &handcards, int cursorIndex)
{
    // update mExtraRowNum first
    mExtraRowNum = Common::Util::GetSegmentNum(handcards.Number()) - 1;
    auto [row, col] = ViewFormatter::GetPosOfPlayerBox(0);
    auto [height, width] = ViewFormatter::GetBaseScaleOfBox(0);
    // int height = GetSelfBoxHeight();
    DrawBorderAndUsername(row, col, width, height - 2 + mExtraRowNum, playerStat.GetUsername());
    DrawHandCards(row, col, width, handcards);
    if (handcards.Number() == 1) {
        DrawUNO();
    }

    if (gameStat.IsMyTurn()) {
        // mView[row + 1][col + width - 3] = '*';
        // show cursor only in the turn of player himself
        auto [cardRow, cardCol] = ViewFormatter::GetPosOfHandCard(cursorIndex, handcards);
        mView[cardRow][cardCol - 1] = '>';
    }
}

void View::DrawSelfTimeIndicatorIfNot()
{
    auto [row, col] = ViewFormatter::GetPosOfPlayerBox(0);
    if (mView[row + 5 + mExtraRowNum][col] != '[') {
        DrawTimeIndicator(0, 0);
    }
}

void View::DrawLastPlayedCard(Card lastPlayedCard)
{
    auto [row, col] = ViewFormatter::GetPosOfLastPlayedCard();
    Copy(row, col, lastPlayedCard.ToString());
}

void View::DrawUNO()
{
    auto [row, col] = ViewFormatter::GetPosOfUNOText('U');
    Copy(row, col, UNO_STR);
}

void View::DrawTimeIndicator(int currentPlayer, int timeElapsed)
{
    auto [row, col] = ViewFormatter::GetPosOfPlayerBox(currentPlayer);
    std::string indicator(18, ' ');
    indicator.front() = '[';
    indicator.back() = ']';
    indicator.replace(1, timeElapsed, timeElapsed, '=');
    indicator[timeElapsed + 1] = '>';
    
    if (currentPlayer != 0) {
        Copy(row + 6, col, indicator);
    }
    else {
        Copy(row + 5 + mExtraRowNum, col, indicator);
    }
}

void View::DrawHandCards(int row, int col, int width, const HandCards &handcards)
{
    if (handcards.Number() > 0) {
        for (int i = 0; i < Common::Util::GetSegmentNum(handcards.Number()); i++) {
            AlignCenter(row + 3 + i, col, width, handcards.ToStringBySegment(i));
        }
    }
    else {
        // you have won
        AlignCenter(row + 3, col, width, "You win!");
    }
}

void View::DrawBorder(int row, int col, int width, int height)
{
    DrawHorizontalBorder(row, col, width);
    DrawHorizontalBorder(row + height + 1, col, width);
    DrawVerticalBorder(row + 1, col, height);
    DrawVerticalBorder(row + 1, col + width - 1, height);
    DrawHorizontalBorder(row + 2, col, width);
}

/**
 * draw the border and username of a box, whose top left corner is located by \p row and \p col,
 * with width (including '+' at both sides) of \p width, height (not including '+' at both sides) 
 * of \p height and username of \p username.
 */
void View::DrawBorderAndUsername(int row, int col, int width, int height, const std::string &username)
{
    DrawBorder(row, col, width, height);
    AlignCenter(row + 1, col, width, username);
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
    for (int i = 0; i < height; i++) {
        mView[row + i][col] = '|';
    }
}

void View::AlignCenter(int row, int col, int width, const std::string &src)
{
    int indent = (width - src.size()) / 2;
    Copy(row, col + indent, src);
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
    int rowNum = ViewFormatter::GetBaseScaleOfView().first + view.mExtraRowNum;
    std::for_each(view.mView.begin(), std::next(view.mView.begin(), rowNum),
        [&os](const std::vector<char> &row) {
            for (auto c : row) {
                os << c;
            }
            os << std::endl;
        }
    );
    return os;
}

}}