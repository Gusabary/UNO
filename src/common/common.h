#pragma once

#include <string>
#include <map>

namespace UNO { namespace Common {

class Common {
public:
    static int mPlayerNum;
    static int mTimeoutPerTurn;
    static int mHandCardsNumPerRow;
    // color escape
    static std::string mRedEscape;
    static std::string mYellowEscape;
    static std::string mGreenEscape;
    static std::string mBlueEscape;
    const static std::map<std::string, std::string> mEscapeMap;
};
}}