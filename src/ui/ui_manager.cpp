#include "ui_manager.h"

namespace UNO { namespace UI {

void UIManager::Render()
{
    // before render, mView should be cleared first
    mView->Clear();
    RenderOthers();
    RenderSelf();
    std::cout << *mView << std::endl;
    std::cout << "Last played card: " << mGameStat->GetLastPlayedCard() << std::endl;
}

void UIManager::RenderOthers()
{
    // start from i == 1, i == 0 is player himself
    for (int i = 1; i < mPlayerStats.size(); i++) {
        bool isCurrentPlayer = false;
        if (mGameStat->GetCurrentPlayer() == i) {
            isCurrentPlayer = true;
        }
        // XXX: 36 * (i - 1) as col, has no scalability to more players
        mView->DrawOtherBox(0, 36 * (i - 1), *mGameStat, mPlayerStats[i], isCurrentPlayer);
    }
}

void UIManager::RenderSelf()
{
    mView->DrawSelfBox(8, 0, *mGameStat, mPlayerStats[0], *mHandCards);
}
}}

