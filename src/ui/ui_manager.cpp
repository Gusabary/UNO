#include "ui_manager.h"

namespace UNO { namespace UI {

void UIManager::Render(bool isMyTurn, bool lastCardCanBePlayed, bool hasChanceToPlayAfterDraw)
{
    // before render, mView should be cleared first
    mView->Clear();
    RenderOthers();
    RenderSelf();

    // before print, it needs to clear screen first
    // ClearScreen();
    std::cout << *mView << std::endl;
    std::cout << "Last played card:" << mGameStat->GetLastPlayedCard() << std::endl;
    
    if (isMyTurn) {
        PrintHintText(lastCardCanBePlayed, hasChanceToPlayAfterDraw);
    }
}

void UIManager::PrintHintText(bool lastCardCanBePlayed, bool hasChanceToPlayAfterDraw)
{
    if (!lastCardCanBePlayed) {
        std::cout << "This card cannot be played. Last played card is "
                  << mGameStat->GetLastPlayedCard() << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else if (!hasChanceToPlayAfterDraw) {
        std::cout << "Now it's your turn." << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else {
        std::cout << "Press Enter to play the card just drawn immediately." << std::endl;
        std::cout << "Or press Space to turn to the next player." << std::endl;
    }
}

std::pair<InputAction, int> UIManager::GetAction(bool lastCardCanBePlayed, bool hasChanceToPlayAfterDraw)
{
    while (true) {
        Render(true, lastCardCanBePlayed, hasChanceToPlayAfterDraw);
        InputAction action = mInputter->GetAction();
        switch (action) {
            case InputAction::CURSOR_MOVE_LEFT: {
                if (!hasChanceToPlayAfterDraw) {
                    mCursorIndex = Common::Util::Wrap(mCursorIndex - 1, mPlayerStats[0].GetRemainingHandCardsNum());
                }
                break;
            }
            case InputAction::CURSOR_MOVE_RIGHT: {
                if (!hasChanceToPlayAfterDraw) {
                    mCursorIndex = Common::Util::Wrap(mCursorIndex + 1, mPlayerStats[0].GetRemainingHandCardsNum());
                }
                break;
            }
            case InputAction::PLAY: {
                /// XXX: the last card as the one just drawn, need to modify if use multiset to store handcards
                int cardIndex = (!hasChanceToPlayAfterDraw) ? mCursorIndex :
                    (mPlayerStats[0].GetRemainingHandCardsNum() - 1);
                return std::make_pair(InputAction::PLAY, cardIndex);
            }
            case InputAction::PASS: {
                return std::make_pair(InputAction::PASS, -1);
            }
            default: assert(0);
        }
    }
}

CardColor UIManager::SpecifyNextColor()
{
    return mInputter->SpecifyNextColor();
}

void UIManager::RenderOthers()
{
    // start from i == 1, since i == 0 is player himself
    for (int i = 1; i < mPlayerStats.size(); i++) {
        mView->DrawOtherBox(i, *mGameStat, mPlayerStats[i]);
    }
}

void UIManager::RenderSelf()
{
    mView->DrawSelfBox(*mGameStat, mPlayerStats[0], *mHandCards, mCursorIndex);
}

void UIManager::ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

}}

