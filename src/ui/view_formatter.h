#pragma once

#include <utility>
#include <vector>

#include "../common/common.h"

namespace UNO { namespace UI {

class ViewFormatter {
public:
    using PosT = std::pair<int, int>;

public:
    ViewFormatter();

    PosT GetPosOfPlayerBox(int playerIndex);

    PosT GetPosOfLastPlayedCard();

private:
    std::vector<std::vector<PosT>> mPosOfPlayerBox;
    std::vector<PosT> mPosOfLastPlayedCard;
};
}}