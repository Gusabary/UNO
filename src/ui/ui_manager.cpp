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
    Common::Util::HideTerminalCursor();
}

void UIManager::RunTimerThread()
{
    mTimerThreadShouldStop = false;
    mTimerThread.reset(new std::thread([this]() { TimerThreadLoop(); }));
}

void UIManager::StopTimerThread()
{
    mTimerThreadShouldStop = true;
    // don't forget to join the thread
    mTimerThread->join();
}

void UIManager::TimerThreadLoop()
{
    while (!mTimerThreadShouldStop) {
        mGameStat->Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::lock_guard<std::mutex> lock(mMutex);
        mView->DrawTimeIndicator(mGameStat->GetCurrentPlayer(), mGameStat->GetTimeElapsed());
        /// XXX: race condition. it may print before main thread prints first time, 
        /// in which case some states are not initialized yet such as mExtraRowNum of View.
        /// current workaround: init mExtraRowNum with 0
        Print(false);
    }
}

void UIManager::RenderWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime)
{
    mView->Clear(true);
    mView->DrawWhenInitWaiting(usernames, isFirstTime);
    mOutputter->PrintRawView(*mView);
}

void UIManager::Render(bool useCls)
{
    std::lock_guard<std::mutex> lock(mMutex);
    // before render, mView should be cleared first
    if (mGameStat->DoesGameEnd()) {
        mView->Clear(true);
    }
    else {
        mView->Clear(false, mGameStat->GetCurrentPlayer());
    }
    mView->DrawSelfBox(*mGameStat, mPlayerStats[0], *mHandCards, mCursorIndex);
    for (int i = 1; i < mPlayerStats.size(); i++) {
        mView->DrawOtherBox(i, *mGameStat, mPlayerStats[i]);
    }
    mView->DrawLastPlayedCard(mGameStat->GetLastPlayedCard());

    Print(useCls);
}

void UIManager::NextTurn()
{
    if (mGameStat->IsMyTurn()) {
        ResetCursor();
        ResetTimeLeft();
        Common::Terminal::ClearStdInBuffer();
    }

    std::lock_guard<std::mutex> lock(mMutex);
    mView->Clear(true);
}

void UIManager::Print(bool useCls) const
{
    // get value only once, for atomicity
    auto isMyTurn = mGameStat->IsMyTurn();
    if (isMyTurn) {
        mView->DrawSelfTimeIndicatorIfNot();
    }
    mOutputter->PrintView(*mView, useCls);

    if (isMyTurn) {
        mOutputter->PrintHintText(mIsSpecifyingNextColor, mLastCardCanBePlayed, 
            mHasChanceToPlayAfterDraw);
    }
}

std::pair<InputAction, int> UIManager::GetAction(bool lastCardCanBePlayed, 
    bool hasChanceToPlayAfterDraw)
{
    mLastCardCanBePlayed = lastCardCanBePlayed;
    mHasChanceToPlayAfterDraw = hasChanceToPlayAfterDraw;
    bool isFirstRender = true;
    while (true) {
        Render(isFirstRender);
        isFirstRender = false;

        InputAction action;
        ExecuteWithTimePassing([this, &action] {
            action = mInputter->GetAction(mTimeLeft);
        });

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
            case InputAction::QUIT: {
                std::cout << "Bye." << std::endl;
                std::exit(0);
            }
            default: assert(0);
        }
    }
}

CardColor UIManager::SpecifyNextColor()
{
    mIsSpecifyingNextColor = true;
    {
        std::lock_guard<std::mutex> lock(mMutex);
        Print();
    }

    CardColor nextColor;
    ExecuteWithTimePassing([this, &nextColor] {
        nextColor = mInputter->SpecifyNextColor(mTimeLeft); 
    });

    mIsSpecifyingNextColor = false;
    return nextColor;
}

bool UIManager::WantToPlayAgain(const std::string &winner)
{
    std::cout << winner << " won. Want to play again? (Y/n) " << std::endl;
    while (true) {
        char ch;
        try {
            ch = Common::Util::GetCharWithTimeout(30000, true);
        }
        catch (const std::exception &e) {
            // timeout, regard it as 'N'
            ch = 'N';
        }

        switch (ch) {
            case 'y': case 'Y': return true;
            case 'n': case 'N': return false;
        }
    }
}

void UIManager::ExecuteWithTimePassing(const std::function<void()> &func)
{
    auto startTime = std::chrono::system_clock::now();
    func();
    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> secondsElapsed = endTime - startTime;
    mTimeLeft -= static_cast<int>(secondsElapsed.count() * 1000);
    // mTimeLeft shouldn't be negative
    mTimeLeft = std::max(mTimeLeft, 0);
}

}}

