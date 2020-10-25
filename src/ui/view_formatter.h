#pragma once

#include <utility>
#include <vector>

#include "../common/common.h"

namespace UNO { namespace UI {

class ViewFormatter {
public:
    using PosT = std::pair<int, int>;
    using ScaleT = std::pair<int, int>;

public:
    ViewFormatter();

    PosT GetPosOfPlayerBox(int playerIndex) const;

    PosT GetPosOfLastPlayedCard() const;

    ScaleT GetBaseScaleOfView() const;

    ScaleT GetMaxScaleOfView() const;

private:
    std::vector<std::vector<PosT>> mPosOfPlayerBox;
    std::vector<PosT> mPosOfLastPlayedCard;
    std::vector<ScaleT> mBaseScaleOfView;
    std::vector<ScaleT> mMaxScaleOfView;
};
}}