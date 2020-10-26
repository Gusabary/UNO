#include "ui_manager.h"

namespace UNO { namespace UI {

UIManager::UIManager(std::unique_ptr<GameStat> &gameStat, 
    std::vector<PlayerStat> &playerStats, std::unique_ptr<HandCards> &handCards) 
    : mGameStat(gameStat), mPlayerStats(playerStats), mHandCards(handCards)
{
    // ViewFormatter should be init first
    ViewFormatter::Init();
    mView.reset(new View());
    mInputter.reset(new Inputter());
    mOutputter.reset(new Outputter(gameStat, playerStats, handCards));
}

void UIManager::RunTimerThread()
{
    mTimerThread.reset(new std::thread([this]() { TimerThreadLoop(); }));
}

void UIManager::TimerThreadLoop()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        mGameStat->Tick();
        mView->DrawTimeIndicator(mGameStat->GetCurrentPlayer(), mGameStat->GetTimeElapsed());
        Print();
    }
}

void UIManager::Render()
{
    // before render, mView should be cleared first
    mView->Clear(false, mGameStat->GetCurrentPlayer());
    RenderOthers();
    RenderSelf();
    mView->DrawLastPlayedCard(mGameStat->GetLastPlayedCard());

    Print();
}

void UIManager::NextTurn()
{
    if (mGameStat->IsMyTurn()) {
        ResetCursor();
        ResetTimeLeft();
    }
    mView->Clear(true);
}

void UIManager::Print() const
{
    // before print, it needs to clear screen first
    // ClearScreen();
    // std::cout << *mView << std::endl;
    mOutputter->PrintView(*mView);

    if (mGameStat->IsMyTurn()) {
        PrintHintText();
    }
}

void UIManager::PrintHintText() const
{
    if (!mLastCardCanBePlayed) {
        std::cout << "This card cannot be played. Last played card is "
                  << mGameStat->GetLastPlayedCard() << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else if (!mHasChanceToPlayAfterDraw) {
        std::cout << "Now it's your turn." << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else {
        std::cout << "Press Enter to play the card just drawn immediately." << std::endl;
        std::cout << "Or press Space to turn to the next player." << std::endl;
    }
}

std::pair<InputAction, int> UIManager::GetAction(bool lastCardCanBePlayed, 
    bool hasChanceToPlayAfterDraw)
{
    mLastCardCanBePlayed = lastCardCanBePlayed;
    mHasChanceToPlayAfterDraw = hasChanceToPlayAfterDraw;
    while (true) {
        Render();

        auto startTime = std::chrono::system_clock::now();
        InputAction action = mInputter->GetAction(mTimeLeft);
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> secondsElapsed = endTime - startTime;
        mTimeLeft -= static_cast<int>(secondsElapsed.count() * 1000);
        // mTimeLeft shouldn't be negative
        mTimeLeft = std::max(mTimeLeft, 0);
        // std::cout << "time left: " << mTimeLeft << std::endl;

        switch (action) {
            case InputAction::CURSOR_MOVE_LEFT: {
                if (!hasChanceToPlayAfterDraw) {
                    mCursorIndex = Common::Util::Wrap(mCursorIndex - 1, 
                        mPlayerStats[0].GetRemainingHandCardsNum());
                }
                break;
            }
            case InputAction::CURSOR_MOVE_RIGHT: {
                if (!hasChanceToPlayAfterDraw) {
                    mCursorIndex = Common::Util::Wrap(mCursorIndex + 1, 
                        mPlayerStats[0].GetRemainingHandCardsNum());
                }
                break;
            }
            case InputAction::PLAY: {
                int cardIndex = (!hasChanceToPlayAfterDraw) ? mCursorIndex :
                    (mPlayerStats[0].GetIndexOfNewlyDrawn());
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
    auto startTime = std::chrono::system_clock::now();
    auto nextColor = mInputter->SpecifyNextColor(mTimeLeft);
    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> secondsElapsed = endTime - startTime;
    mTimeLeft -= static_cast<int>(secondsElapsed.count() * 1000);
    return nextColor;
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

void UIManager::ClearScreen() const
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

}}

