#pragma once

#include <vector>
#include <utility>

#include "../game/stat.h"
#include "view.h"
#include "inputter.h"

namespace UNO { namespace UI {

using namespace Game;

class UIManager {
public:
    UIManager(std::unique_ptr<GameStat> &gameStat, 
        std::vector<PlayerStat> &playerStats,
        std::unique_ptr<HandCards> &handCards) 
        : mGameStat(gameStat), mPlayerStats(playerStats),
        mHandCards(handCards), mView(std::make_unique<View>()),
        mInputter(std::make_unique<Inputter>()) {}

    void Render(bool isMyTurn, bool lastCardCanBePlayed = true,
        bool hasChanceToPlayAfterDraw = false);

    std::pair<InputAction, int> GetAction(bool lastCardCanBePlayed, 
        bool hasChanceToPlayAfterDraw);

    CardColor SpecifyNextColor();

    void ResetCursor() { MoveCursorTo(0); }

    void MoveCursorTo(int index) { mCursorIndex = index; }

private:
    int PlayerNum() const { return mPlayerStats.size(); }

    void RenderSelf();

    void RenderOthers();

    void ClearScreen();

    void PrintHintText(bool lastCardCanBePlayed, bool hasChanceToPlayAfterDraw);

private:
    std::unique_ptr<View> mView;
    std::unique_ptr<Inputter> mInputter;

    std::unique_ptr<GameStat> &mGameStat;
    std::vector<PlayerStat> &mPlayerStats;
    std::unique_ptr<HandCards> &mHandCards;
    int mCursorIndex{0};
};
}}