#pragma once

#include <vector>
#include <utility>
#include <thread>

#include "view.h"
#include "inputter.h"
#include "outputter.h"

namespace UNO { namespace UI {

using namespace Game;

class UIManager {
public:
    UIManager(std::unique_ptr<GameStat> &gameStat,
              std::vector<PlayerStat> &playerStats,
              std::unique_ptr<HandCards> &handCards);

    ~UIManager() { mTimerThread->join(); }

    void RunTimerThread();

    void StopTimerThread();

    void RenderWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime);

    void Render();

    std::pair<InputAction, int> GetAction(bool lastCardCanBePlayed, 
        bool hasChanceToPlayAfterDraw);

    CardColor SpecifyNextColor();

    void NextTurn();

    void MoveCursorTo(int index) { mCursorIndex = index; }

private:
    void TimerThreadLoop();

    void Print() const;

    void ResetCursor() { MoveCursorTo(0); }

    void ResetTimeLeft() { mTimeLeft = Common::Common::mTimeoutPerTurn * 1000; }

    int PlayerNum() const { return mPlayerStats.size(); }

    void ExecuteWithTimePassing(const std::function<void()> &func);

private:
    std::unique_ptr<View> mView;
    std::unique_ptr<Inputter> mInputter;
    std::unique_ptr<Outputter> mOutputter;

    std::unique_ptr<GameStat> &mGameStat;
    std::vector<PlayerStat> &mPlayerStats;
    std::unique_ptr<HandCards> &mHandCards;
    int mCursorIndex{0};
    int mTimeLeft;  // in milliseconds

    std::unique_ptr<std::thread> mTimerThread;

    bool mLastCardCanBePlayed;
    bool mHasChanceToPlayAfterDraw;
    bool mIsSpecifyingNextColor;

    bool mTimerThreadShouldStop{false};
    // when printing view, it shouldn't be modified
    std::mutex mMutex;
};
}}