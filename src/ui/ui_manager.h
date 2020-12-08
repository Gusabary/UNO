#pragma once

#include <vector>
#include <utility>
#include <thread>
#include <functional>
#include <mutex>

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

    // ~UIManager() { mTimerThread->join(); }

    /**
     * Start the timer thread.
     */
    void RunTimerThread();

    /**
     * Stop the timer thread and wait for joining.
     */
    void StopTimerThread();

    /**
     * Render view when waiting for other players to join.
     */
    void RenderWhenInitWaiting(const std::vector<std::string> &usernames, bool isFirstTime);

    /**
     * Render view in the main game loop.
     */
    void Render(bool useCls = true);

    /**
     * Get the player's action.
     */
    std::pair<InputAction, int> GetAction(bool lastCardCanBePlayed, 
        bool hasChanceToPlayAfterDraw);

    /**
     * Get the specified next color when a 'W' or '+4' is played.
     */
    CardColor SpecifyNextColor();

    /**
     * Ask player whether want to play again.
     */
    bool WantToPlayAgain(const std::string &winner);

    /**
     * Hook invoked when the game enters next turn.
     */
    void NextTurn();

    /**
     * Move the cursor in handcard to the position indicated by \param index.
     */
    void MoveCursorTo(int index) { mCursorIndex = index; }

private:
    void TimerThreadLoop();

    void Print(bool useCls = true) const;

    void ResetCursor() { MoveCursorTo(0); }

    void ResetTimeLeft() { mTimeLeft = Common::Common::mTimeoutPerTurn * 1000 + 500; }

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
    bool mIsSpecifyingNextColor{false};

    bool mTimerThreadShouldStop{false};
    // when printing view, it shouldn't be modified
    std::mutex mMutex;
};
}}