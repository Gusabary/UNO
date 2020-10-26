#pragma once

#include <utility>
#include <vector>

#include "../common/util.h"
#include "../game/cards.h"

namespace UNO { namespace UI {

class ViewFormatter {
public:
    using PosT = std::pair<int, int>;
    using ScaleT = std::pair<int, int>;

public:
    static void Init();
    
    static PosT GetPosOfPlayerBox(int playerIndex);

    static PosT GetPosOfLastPlayedCard();

    static PosT GetPosOfPlayerLastPlayedCard(int playerIndex);

    static PosT GetPosOfHandCard(int handcardIndex, const Game::HandCards &handcards);

    static ScaleT GetBaseScaleOfView();

    static ScaleT GetMaxScaleOfView();

    static ScaleT GetBaseScaleOfBox(int playerIndex);

private:
    static std::vector<std::vector<PosT>> mPosOfPlayerBox;
    static std::vector<PosT> mPosOfLastPlayedCard;
    static std::vector<ScaleT> mBaseScaleOfView;
};
}}